#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <platform/gtui.h>

#ifdef SHOW_CONSOLE
#include <platform/console_signals.hpp>
#endif

#include <tracy/Tracy.hpp>

#include <core/application.hpp>
#include <core/logger.hpp>
#include <subsystems/render_interface.hpp>

#include <GLFW/glfw3.h>

//ImGui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#ifdef __APPLE__
#include <backends/imgui_impl_metal.h>
#else
//#include <backends/imgui_impl_vulkan.h>
#ifdef _WIN32
#include <backends/imgui_impl_dx12.h>
#endif
#endif

#ifdef CLOUD_EDITOR
#include <editor/editor.hpp>
#else
#include <game/game.hpp>
#endif

/*
* Initialization flags(in order of initialization)
* The logger will be the first to be initialized, each other module can use safely its functions.
* The modules will be terminated from last to first, so each one is able call log functions for its clean up stage.
*/
#define INITIALIZED_LOGGER   0b00000001
#define INITIALIZED_GLFW     0b00000010
#define INITIALIZED_RENDERER 0b00000100
#define INITIALIZED_IMGUI	 0b00001000

namespace Application
{
    static int32_t _initializedModules;
    static bool _appLaunched = false;
    static StartupSettings _settings;
    static bool _settingsInitialized = false;

    static bool InitializeModules();
    static void TerminateModules();
    static void GLFWErrorCallback(int error, const char* description);

    bool Run()
    {
        if (_appLaunched) return true;

        tracy::SetThreadName("Main Thread");

        #ifdef SHOW_CONSOLE
        SetupSignalHandlers(); //Correctly terminate the program even when the console is trying to close it before proper clean up.
        #endif

        srand(time(nullptr));

#pragma region Initialize Settings
        //Load settings file
        FILE* settingsFile = fopen(SETTINGS_FILE_PATH, "a+");
        if (!settingsFile)
        {
            #ifdef SHOW_CONSOLE
            printf("Failed to open the settings file at %s\r\n", SETTINGS_FILE_PATH);
            #endif
            return false;
        }

        //Read through properties, if a property is not found, or is incorrectly named, the default one will be used instead, defined in the StartupSettings constructor
        while (true)
        {
            char property[100] = { 0 };
            char value[100] = { 0 };
            if (fscanf(settingsFile, "%s - %s\n", property, value) == EOF)
                break;

            size_t valueLen = strlen(value);
            if (valueLen == 0)
                continue;

            if (strcmp(property, "LoggerFilePath") == 0)
            {
                _settings.loggerFilePath = value;
            }

            else if (strcmp(property, "GraphicsAPI") == 0)
            {
                if (strcmp(value, "OpenGL") == 0)
                { _settings.graphicsAPI = GraphicsAPI::OpenGL; }
                else if (strcmp(value, "Vulkan") == 0)
                { _settings.graphicsAPI = GraphicsAPI::Vulkan; }
                else if (strcmp(value, "DirectX12") == 0)
                { _settings.graphicsAPI = GraphicsAPI::DirectX12; }
                else if (strcmp(value, "Metal") == 0)
                { _settings.graphicsAPI = GraphicsAPI::Metal; }
                else { /*The default value will be used*/ }
            }

            else if (strcmp(property, "OpenGL_Requested") == 0)
            {
                if (valueLen != 3) //"<major>.<minor>"
                    continue; //Use the default one
                _settings.openGLMajorVersionRequested = value[0] - '0';
                _settings.openGLMinorVersionRequested = value[2] - '0';

                if (_settings.openGLMajorVersionRequested < OPENGL_MAJOR_REQUIRED)
                {
                    //Note that the minimum required opengl version will be used
                    _settings.openGLMajorVersionRequested = OPENGL_MAJOR_REQUIRED;
                    _settings.openGLMinorVersionRequested = OPENGL_MINOR_REQUIRED;
                    #ifdef SHOW_CONSOLE
                    printf(
                        GTUI_ESC_ENABLE_BOLD GTUI_ESC_FG_YELLOW
                        "Application::Initialize -- the requested opengl version was below the minimum requirements, "
                        "so the %d.%d version will be requested instead.\r\n"
                        "Note that the final resulting version might be higher, depending on glfw implementations\r\n."
                        GTUI_ESC_DISABLE_BOLD GTUI_ESC_FG_DEFAULT,
                        OPENGL_MAJOR_REQUIRED, OPENGL_MINOR_REQUIRED);
                    #endif
                }

                else if (_settings.openGLMajorVersionRequested == OPENGL_MAJOR_REQUIRED && _settings.openGLMinorVersionRequested < OPENGL_MINOR_REQUIRED)
                {
                    //Note that the minimum required opengl version will be used
                    _settings.openGLMinorVersionRequested = OPENGL_MINOR_REQUIRED;
                    #ifdef SHOW_CONSOLE
                    printf(
                        GTUI_ESC_ENABLE_BOLD GTUI_ESC_FG_YELLOW
                        "Application::Initialize -- the requested opengl version was below the minimum requirements, "
                        "so the %d.%d version will be requested instead.\r\n"
                        "Note that the final resulting version might be higher, depending on glfw implementations\r\n."
                        GTUI_ESC_DISABLE_BOLD GTUI_ESC_FG_DEFAULT,
                        OPENGL_MAJOR_REQUIRED, OPENGL_MINOR_REQUIRED);
                    #endif
                }
            }

            else if (strcmp(property, "UIStyle") == 0)
            {
                if (strcmp(value, "ImGuiClassic") == 0)
                {
                    _settings.imGuiStyle = UIStyle::ImGuiClassic;
                }

                else if (strcmp(value, "ImGuiDark") == 0)
                {
                    _settings.imGuiStyle = UIStyle::ImGuiDark;
                }

                else if (strcmp(value, "ImGuiLight") == 0)
                {
                    _settings.imGuiStyle = UIStyle::ImGuiLight;
                }

                else { /*Default style will be used*/ }
            }
        }

        #ifdef __APPLE__

        if (_settings.graphicsAPI != GraphicsAPI::Metal &&
	    _settings.graphicsAPI != GraphicsAPI::OpenGL)
        {
            #ifdef SHOW_CONSOLE
            printf("On MacOS, Metal or OpenGL graphics APIs are required.\r\n");
            #endif
            return false;
        }

        #elif __linux__

        if (_settings.graphicsAPI == GraphicsAPI::DirectX12 ||
	    _settings.graphicsAPI == GraphicsAPI::Metal)
        {
            #ifdef SHOW_CONSOLE
            printf("On linux, DirectX12 and Metal graphics APIs are not supported.\r\n");
            #endif
            return false;
        }

        #elif _WIN32

    	if (_settings.graphicsAPI == GraphicsAPI::Metal)
    	{
    	    #ifdef SHOW_CONSOLE
    	    printf("On windows, Metal graphics API is not supported\r\n");
    	    #endif
    	    return false;
    	}

        #endif

        fclose(settingsFile);

        _settingsInitialized = true;
#pragma endregion

        //Initialize submodules
        bool initializationResult = InitializeModules();
        if (!initializationResult)
        {
            if (_initializedModules & INITIALIZED_LOGGER)
            {
                Logger::PushMessage("Main", "Failed to initialize Cloud", Logger::Fatal);
            }

            else
            {
                #ifdef SHOW_CONSOLE
                printf("Failed to initalize logger\r\n");
                #endif
            }

            return false;
        }

        glfwSetErrorCallback(GLFWErrorCallback);

        _appLaunched = true;

        //Start the Main Loop
        #ifdef CLOUD_EDITOR
        Logger::PushMessage("Main", "Editor not yet implemented", Logger::Fatal);
        #else
        Game::InitializeWithoutEditor();
        while (Game::RunWithoutEditor());
        Game::TerminateWithoutEditor();
        #endif

        TerminateModules();

        return true;
    }

    static bool InitializeModules()
    {
        if (!Logger::Initialize(_settings.loggerFilePath))
        {
#ifdef SHOW_CONSOLE
            printf("Failed to initialize Logger.\r\nEngine closing...\r\n");
#endif
            return false;
        }

        _initializedModules |= INITIALIZED_LOGGER;

        if (!glfwInit())
        {
            Logger::PushMessage("Main::Initialize", "failed to initialize glfw", Logger::Fatal);
            return false;
        }

        _initializedModules |= INITIALIZED_GLFW;

        if (!Renderer::Initialize(_settings.graphicsAPI))
        {
            Logger::PushMessage("Main::Initialize", "failed to initialize renderer", Logger::Fatal);
            return false;
        }

        _initializedModules |= INITIALIZED_RENDERER;

        //Initialize ImGui based on the graphics api used
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        switch (_settings.imGuiStyle)
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

        switch (_settings.graphicsAPI)
        {
        case GraphicsAPI::OpenGL:
            //TODO: glsl versions do not match exactly the opengl version. For example, opengl 3.3 uses 3.30 glsl, while opengl 3.0 uses 1.50 glsl.
            //This direct conversion does not work well always, but for now its fine.
            glVersion.append("#version ");
            glVersion.append(std::to_string(_settings.openGLMajorVersionRequested));
            glVersion.append(std::to_string(_settings.openGLMinorVersionRequested));
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

        _initializedModules |= INITIALIZED_IMGUI;

        return true;
    }

    static void TerminateModules()
    {
        if (_initializedModules & INITIALIZED_IMGUI)
        {
            switch (_settings.graphicsAPI)
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

        _initializedModules &= ~INITIALIZED_IMGUI;

        if (_initializedModules & INITIALIZED_RENDERER) Renderer::Terminate();
        _initializedModules &= ~INITIALIZED_RENDERER;
        if (_initializedModules & INITIALIZED_GLFW) glfwTerminate();
        _initializedModules &= ~INITIALIZED_GLFW;
        if (_initializedModules & INITIALIZED_LOGGER) Logger::Terminate();
        _initializedModules &= ~INITIALIZED_LOGGER;
    }

    bool IsModuleInitialized(int moduleFlag)
    {
        return _initializedModules & moduleFlag;
    }

    static void GLFWErrorCallback(int error, const char* description)
    {
        Logger::PushMessage("Application::GLFWErrorCallback", description, Logger::Error);
    }

    const StartupSettings& GetSettings()
    {
        assert(_settingsInitialized);

        return _settings;
    }
}