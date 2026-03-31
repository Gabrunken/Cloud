#include <subsystems/render_interface.hpp>
#include <glad/glad.h>
#include <core/logger.hpp>

static RenderContext currentRenderingContext;
static LogCallback logCallback;

class OpenGLRenderer : public RenderInterface
{
public:
	OpenGLRenderer()
	{


		logCallback("OpenGLRenderer::Constructor", "renderer initialized", Logger::Success);
	}

	~OpenGLRenderer()
	{
		logCallback("OpenGLRenderer::Destructor", "renderer destroyed", Logger::Success);
	}

	void SetBackgroundColor(const Color& color)
	{
		//glClearColor(color.r, color.g, color.b, color.a);
		currentRenderingContext.backgroundColor = color;
	}

	void ClearBackground()
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
};

extern "C" RENDERER_API RenderInterface* CreateRenderer(LogCallback callback)
{
	logCallback = callback;
	return new OpenGLRenderer();
}