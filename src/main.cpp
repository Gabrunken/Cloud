/*
* Application Entry Point
*/

#include <stdlib.h>
#include <core/logger.hpp>
#ifdef SHOW_CONSOLE
#include <platform/console_signals.hpp>
#endif
#include <GLFW/glfw3.h> //cross-platform window and input handling
#include <tracy/Tracy.hpp> //profiler
#include <subsystems/render_interface.hpp>

#ifdef CLOUD_EDITOR
#include <editor/editor.hpp>
#else
#include <game/game.hpp>
#endif

#define LOGGER_FILE_PATH "log.txt"
#define GRAPHICS_API GraphicsAPI::OpenGL

/*
* Initialization flags(in order of initialization)
* The logger will be the first to be initialized, each other module can use safely its functions.
* The modules will be terminated from last to first, so each one is able call log functions for its clean up stage.
*/
#define INITIALIZED_LOGGER   0b00000001
#define INITIALIZED_GLFW     0b00000010
#define INITIALIZED_RENDERER 0b00000100

static int32_t initializedModules;

static bool Initialize(); //Returns false at the first module's initialization failure.
static void Terminate(); //Will only terminate the modules that were fully initialized from last to first.

int main(int argc, char** argv)
{
	tracy::SetThreadName("Main Thread");

	atexit(Terminate); //Terminate will be automatically called when the main function returns.

	#ifdef SHOW_CONSOLE
	SetupSignalHandlers(); //Correctly terminate the program even when the console is trying to close it before proper clean up.
	#endif

	bool initializationResult = Initialize();
	if (!initializationResult)
	{
		if (initializedModules & INITIALIZED_LOGGER)
		{
			Logger::PushMessage("Main", "Failed to initialize Cloud", Logger::Fatal);
		}

		else
		{
			#ifdef SHOW_CONSOLE
			printf("Failed to initialize Cloud.\n");
			#endif
		}

		return 1;
	}

#ifdef CLOUD_EDITOR
	Logger::PushMessage("Main", "Editor not yet implemented", Logger::Fatal);
#else
	Game::InitializeWithoutEditor();
	while (Game::RunWithoutEditor());
	Game::TerminateWithoutEditor();
#endif
}

bool Initialize()
{
	if (!Logger::Initialize(LOGGER_FILE_PATH)) return false;
	initializedModules |= INITIALIZED_LOGGER;

	if (!glfwInit()) return false;
	initializedModules |= INITIALIZED_GLFW;

	if (!Renderer::Initialize(GRAPHICS_API)) return false;
	initializedModules |= INITIALIZED_RENDERER;

	return true;
}

void Terminate()
{
	if (initializedModules & INITIALIZED_RENDERER) Renderer::Terminate();
	if (initializedModules & INITIALIZED_GLFW) glfwTerminate();
	if (initializedModules & INITIALIZED_LOGGER) Logger::Terminate();
}
