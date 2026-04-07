#include <subsystems/render_interface.hpp>
#include <core/logger.hpp>

class DirectX12RenderContext : public IRenderContext
{

};

static DirectX12RenderContext currentRenderingContext;
static LogCallback logCallback;
static GraphicsAPILoader loaderCallback;

class DX12Renderer : public RenderInterface
{
public:
	DX12Renderer()
	{
		logCallback("DX12Renderer::Constructor", "renderer initialized", Logger::Success);
	}

	~DX12Renderer()
	{
		logCallback("DX12Renderer::Destructor", "renderer destroyed", Logger::Success);
	}

	void SetBackgroundColor(const Color& color)
	{
		currentRenderingContext.backgroundColor = color;
	}

	void ClearBackground()
	{

	}

	void Present()
	{

	}
};

extern "C" RENDERER_API RenderInterface* CreateRenderer(LogCallback log, GraphicsAPILoader loader)
{
	logCallback = log;
	loaderCallback = loader;
	return new DX12Renderer();
}

extern "C" RENDERER_API void DestroyRenderer(RenderInterface* ptr)
{
	if (!ptr)
	{
		logCallback("DX12Renderer::DestroyRenderer", "the passed renderer was null", Logger::Fatal);
		return;
	}

	DX12Renderer* renderer = dynamic_cast<DX12Renderer*>(ptr);
	if (!renderer)
	{
		logCallback("DX12Renderer::DestroyRenderer", "the passed renderer is not an OpenGL Renderer, use the right function to delete it", Logger::Fatal);
		return;
	}

	delete renderer;
}