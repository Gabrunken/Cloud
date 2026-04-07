#include <subsystems/render_interface.hpp>
#include <core/logger.hpp>
#include <glad/glad.h>

static RenderContext currentRenderingContext;
static LogCallback logCallback;
static GraphicsAPILoader loaderCallback;

class OpenGLRenderer : public RenderInterface
{
public:
	OpenGLRenderer()
	{
		//Load opengl with glad
		gladLoadGLLoader((GLADloadproc)loaderCallback);

		logCallback("OpenGLRenderer::Constructor", "renderer initialized", Logger::Success);
	}

	~OpenGLRenderer()
	{
		logCallback("OpenGLRenderer::Destructor", "renderer destroyed", Logger::Success);
	}

	void SetBackgroundColor(const Color& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
		currentRenderingContext.backgroundColor = color;
	}

	void ClearBackground()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Present()
	{

	}
};

extern "C" RENDERER_API RenderInterface* CreateRenderer(LogCallback log, GraphicsAPILoader loader)
{
	logCallback = log;
	loaderCallback = loader;
	return new OpenGLRenderer();
}
 
extern "C" RENDERER_API void DestroyRenderer(RenderInterface* ptr)
{
	if (!ptr)
	{
		logCallback("OpenGLRenderer::DestroyRenderer", "the passed renderer was null", Logger::Fatal);
		return;
	}

	OpenGLRenderer* renderer = dynamic_cast<OpenGLRenderer*>(ptr);
	if (!renderer)
	{
		logCallback("OpenGLRenderer::DestroyRenderer", "the passed renderer is not an OpenGL Renderer, use the right function to delete it", Logger::Fatal);
		return;
	}

	delete renderer;
}