#include <subsystems/render_interface.hpp>
#include <core/logger.hpp>

static RenderContext currentRenderingContext;
static LogCallback logCallback;

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
};

extern "C" RENDERER_API RenderInterface* CreateRenderer(LogCallback callback)
{
	logCallback = callback;
	return new DX12Renderer();
}