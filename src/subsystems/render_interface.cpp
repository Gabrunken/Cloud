#include <subsystems/render_interface.hpp>
#include <core/utils.hpp>
#include <platform/shared_library.hpp>

static GraphicsAPI currentGraphicsAPI; //This should only be initialized one time and left as it is for the whole runtime.
static RenderInterface* rendererInterface;
static LibraryHandle graphicsLib = nullptr;
static bool _initialized = false;

namespace Renderer
{
	bool Initialize(GraphicsAPI graphicsAPI)
	{
		CloudAssert(!_initialized, "Renderer::Initialize", "already initialized");
		currentGraphicsAPI = graphicsAPI;

		//Load library		
		switch (graphicsAPI)
		{
		case GraphicsAPI::OpenGL:
			graphicsLib = LoadSharedLibrary(SHARED_LIB_PREFIX "OpenGLRenderer" SHARED_LIB_SUFFIX);
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
		rendererInterface = func(reinterpret_cast<LogCallback>(Logger::PushMessage));
		
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