#include <subsystems/render_interface.hpp>
#include <core/utils.hpp>
#include <platform/shared_library.hpp>
#include <GLFW/glfw3.h>
#include <core/application.hpp>
//#include <glad/glad.h>

static RenderInterface* rendererInterface;
static LibraryHandle graphicsLib = nullptr;
static bool _initialized = false;

namespace Renderer
{
	bool Initialize(GraphicsAPI graphicsAPI)
	{
		CloudAssert(!_initialized, "Renderer::Initialize", "already initialized");

		void* apiLoader = nullptr;

		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
		
		//Quick OpenGL check for glfw
		if (graphicsAPI == GraphicsAPI::OpenGL)
		{
			const StartupSettings& settings = Application::GetSettings();

			if (graphicsAPI == GraphicsAPI::OpenGL)
			{
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.openGLMajorVersionRequired);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.openGLMinorVersionRequired);

				#ifdef __APPLE__
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
				#endif
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			}

			else
			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			}
		}

		GLFWwindow* window = glfwCreateWindow(400, 400, "no title", nullptr, nullptr);
		if (!window) return false;

		//Load library		
		switch (graphicsAPI)
		{
		case GraphicsAPI::OpenGL:
			graphicsLib = LoadSharedLibrary(SHARED_LIB_PREFIX "OpenGLRenderer" SHARED_LIB_SUFFIX);
			apiLoader = glfwGetProcAddress;
			glfwMakeContextCurrent(window);
			break;
		case GraphicsAPI::Vulkan:
			graphicsLib = LoadSharedLibrary(SHARED_LIB_PREFIX "VulkanRenderer" SHARED_LIB_SUFFIX);
			break;
		case GraphicsAPI::DirectX12:
			graphicsLib = LoadSharedLibrary(SHARED_LIB_PREFIX "DX12Renderer" SHARED_LIB_SUFFIX);
			break;
		case GraphicsAPI::Metal:
			graphicsLib = LoadSharedLibrary(SHARED_LIB_PREFIX "MetalRenderer" SHARED_LIB_SUFFIX);
			break;
		default:
			Logger::PushMessage("Renderer::Initialize", "you passed an invalid graphics api", Logger::Fatal);
			return false;
			break;
		}

		if (!graphicsLib)
		{
			Logger::PushMessage("Renderer::Initialize", "failed to load the renderer library", Logger::Fatal);
			return false;
		}

		void* symbol = GetLibrarySymbol(graphicsLib, "CreateRenderer");
		if (!symbol)
		{
			Logger::PushMessage("Renderer::Initialize", "failed to find the 'CreateRenderer' symbol inside the renderer shared library", Logger::Fatal);
			return false;
		}

		CreateRendererFunc func = reinterpret_cast<CreateRendererFunc>(symbol);
		rendererInterface = func(reinterpret_cast<LogCallback>(Logger::PushMessage), reinterpret_cast<GraphicsAPILoader>(apiLoader));
		
		Logger::PushMessage("Renderer::Initialize", "initialized successfully", Logger::Success);
		_initialized = true;
		return true;
	}

	RenderInterface& GetRenderer()
	{
		CloudAssert(rendererInterface, "Renderer::GetRenderer", "renderer not created, call 'CreateRenderer()'");
		return *rendererInterface;
	};

	void Terminate()
	{
		CloudAssert(_initialized, "Renderer::Terminate", "has to be initialized");
		
		//Destroy renderer
		void* symbol = GetLibrarySymbol(graphicsLib, "DestroyRenderer");
		if (!symbol)
		{
			Logger::PushMessage("Renderer::Terminate", "failed to find the 'DestroyRenderer' symbol inside the renderer shared library", Logger::Fatal);
			return;
		}

		DestroyRendererFunc func = reinterpret_cast<DestroyRendererFunc>(symbol);
		func(rendererInterface);
		rendererInterface = nullptr;

		//Free library
		UnloadSharedLibrary(graphicsLib);

		Logger::PushMessage("Renderer::Terminate", "terminated successfully", Logger::Success);
	}
}