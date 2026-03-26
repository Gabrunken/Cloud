/*
* Application Entry Point
*/

#include <stdlib.h>
#include <core/logger.hpp>
#include <glad/glad.h> //opengl function loader
#include <GLFW/glfw3.h> //cross-platform window and input handling
#include <tracy/Tracy.hpp> //profiler

#define LOGGER_FILE_PATH "log.txt"

/*
* Initialization flags(in order of initialization)
* The logger will be the first to be initialized, each other module can use safely its functions.
* The modules will be terminated from last to first, so each one is able call log functions for its clean up stage.
*/
#define INITIALIZED_LOGGER 0x00000001
#define INITIALIZED_GLFW   0x00000002

static int32_t initializedModules;

static bool Initialize(); //Returns false at the first module's initialization failure.
static bool MainLoop(); //Returns true if the loop should continue.
static void Terminate(); //Will only terminate the modules that were fully initialized from last to first.

int main(int argc, char** argv)
{
	tracy::SetThreadName("Main Thread");

	atexit(Terminate); //Terminate will be automatically called when the main function returns.

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
	
	while (MainLoop());
}

bool Initialize()
{
	if (!Logger::Initialize(LOGGER_FILE_PATH)) return false;
	initializedModules |= INITIALIZED_LOGGER;

	if (!glfwInit()) return false;
	initializedModules |= INITIALIZED_GLFW;

	return true;
}

bool MainLoop()
{
	ZoneScopedN("Main Loop");
	return false;
}

void Terminate()
{
	if (initializedModules & INITIALIZED_GLFW) glfwTerminate();
	if (initializedModules & INITIALIZED_LOGGER) Logger::Terminate();
}