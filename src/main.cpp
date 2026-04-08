#include <stdlib.h>
#include <core/logger.hpp>
#ifdef SHOW_CONSOLE
#include <platform/console_signals.hpp>
#endif
#include <GLFW/glfw3.h> //cross-platform window and input handling
#include <tracy/Tracy.hpp> //profiler
#include <subsystems/render_interface.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
//#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_metal.h>

#ifdef CLOUD_EDITOR
#include <editor/editor.hpp>
#else
#include <game/game.hpp>
#endif

#include <core/application.hpp>

#define SETTINGS_FILE_PATH "settings.txt"

/*
* Initialization flags(in order of initialization)
* The logger will be the first to be initialized, each other module can use safely its functions.
* The modules will be terminated from last to first, so each one is able call log functions for its clean up stage.
*/
#define INITIALIZED_LOGGER   0b00000001
#define INITIALIZED_GLFW     0b00000010
#define INITIALIZED_RENDERER 0b00000100
#define INITIALIZED_IMGUI	 0b00001000

static int32_t initializedModules;

static bool Initialize(); //Returns false at the first module's initialization failure.
static void Terminate(); //Will only terminate the modules that were fully initialized from last to first.

/*
* ============= ENTRY POINT =============
*/
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
	if (!Application::Initialize(SETTINGS_FILE_PATH)) exit(1);
	const StartupSettings& settings = Application::GetSettings();

	if (!Logger::Initialize(Application::GetSettings().loggerFilePath))
	{
		#ifdef SHOW_CONSOLE
		printf("Failed to initialize Logger.\nEngine closing...");
		#endif
		return false;
	}
	initializedModules |= INITIALIZED_LOGGER;

	if (!glfwInit())
	{
		Logger::PushMessage("Main::Initialize", "failed to initialize glfw", Logger::Fatal);
		return false;
	}
	initializedModules |= INITIALIZED_GLFW;

	if (!Renderer::Initialize(Application::GetSettings().graphicsAPI))
	{
		Logger::PushMessage("Main::Initialize", "failed to initialize renderer", Logger::Fatal);
		return false;
	}
	initializedModules |= INITIALIZED_RENDERER;

	//Initialize ImGUI based on the graphics api used
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	switch (settings.imGuiStyle)
	{
	case UIStyle::ImGuiClassic:
		ImGui::StyleColorsClassic();
		break;
	case UIStyle::ImGuiDark:
		ImGui::StyleColorsDark();
		break;
	case UIStyle::ImGuiLight:
		ImGui::StyleColorsLight();
		break;
	}
	
	std::string glVersion;
	
	switch (settings.graphicsAPI)
	{
	case GraphicsAPI::OpenGL:
		glVersion.append("#version ");
		glVersion.append(std::to_string(settings.openGLMajorVersionRequired));
		glVersion.append(std::to_string(settings.openGLMinorVersionRequired));
		glVersion.append("0 core");
		ImGui_ImplOpenGL3_Init(glVersion.c_str());
		break;
	case GraphicsAPI::Vulkan:
		//TODO
		break;
	case GraphicsAPI::DirectX12:
		//TODO
		break;
	case GraphicsAPI::Metal:
		//TODO
		break;
	default:
		return false;
	}

	initializedModules |= INITIALIZED_IMGUI;

	return true;
}

void Terminate()
{
	if (initializedModules & INITIALIZED_IMGUI)
	{
		switch (Application::GetSettings().graphicsAPI)
		{
		case GraphicsAPI::OpenGL:
			ImGui_ImplOpenGL3_Shutdown();
			break;
		case GraphicsAPI::Vulkan:
			//TODO
			break;
		case GraphicsAPI::DirectX12:
			//TODO
			break;
		case GraphicsAPI::Metal:
			//TODO
			break;
		}

		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	if (initializedModules & INITIALIZED_RENDERER) Renderer::Terminate();
	if (initializedModules & INITIALIZED_GLFW) glfwTerminate();
	if (initializedModules & INITIALIZED_LOGGER) Logger::Terminate();
}
