#include <subsystems/render_interface.hpp>
#include <core/logger.hpp>
#include <glad/glad.h>

class OpenGLRenderContext : public RenderContext
{
public:
	const char* vendor = "null";
	const char* renderer = "null";
	const char* version = "null";
	const char* glslVers = "null";
};

static OpenGLRenderContext currentRenderingContext;
static LogCallback logCallback;
static GraphicsAPILoader loaderCallback;

class OpenGLRenderer : public RenderInterface
{
public:
	OpenGLRenderer()
	{
		//Load opengl with glad
		gladLoadGLLoader((GLADloadproc)loaderCallback);

		currentRenderingContext.vendor = (const char*)glGetString(GL_VENDOR);
		currentRenderingContext.renderer = (const char*)glGetString(GL_RENDERER);
		currentRenderingContext.version = (const char*)glGetString(GL_VERSION);
		currentRenderingContext.glslVers = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

		std::string detailString = "renderer details:\n\tVendor: ";
		detailString.append(currentRenderingContext.vendor);
		detailString.append("\n\tRenderer: ");
		detailString.append(currentRenderingContext.renderer);
		detailString.append("\n\tVersion: ");
		detailString.append(currentRenderingContext.version);
		detailString.append("\n\tGLSL Version: ");
		detailString.append(currentRenderingContext.glslVers);

		logCallback("OpenGLRenderer::Constructor", detailString, Logger::Note);

		logCallback("OpenGLRenderer::Constructor", "renderer initialized", Logger::Success);
	}

	~OpenGLRenderer()
	{
		logCallback("OpenGLRenderer::Destructor", "renderer destroyed", Logger::Success);
	}

	void SetBackgroundColor(const Color& color)
	{
		if (currentRenderingContext.backgroundColor == color)
			return;
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