#include <subsystems/render_interface.hpp>
#include <core/logger.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static RenderContext currentRenderingContext;
static LogCallback logCallback;

class OpenGLRenderer : public RenderInterface
{
public:
	OpenGLRenderer()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); //MacOS requires this
		#endif

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