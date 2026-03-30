#include <subsystems/renderer.hpp>
#include <core/logger.hpp>
#include <glad/glad.h>
#include <core/utils.hpp>

static bool _hasBeenInitialized = false;

namespace Renderer
{
	bool Initialize(GraphicsAPI graphicsAPI)
	{
		CloudAssert(!_hasBeenInitialized, "Renderer::Initialize", "already initialized");
		currentGraphicsAPI = graphicsAPI;
		_hasBeenInitialized = true;
		return true;
	}

	void Terminate()
	{
	
	}

	void SetBackgroundColor(const Color& color)
	{
		CloudAssert(_hasBeenInitialized, "Renderer::SetBackgroundColor", "has not been initialized");
		switch (currentGraphicsAPI)
		{
		case GraphicsAPI::OpenGL:
			glClearColor(color.r, color.g, color.b, color.a);
			currentRenderingContext.backgroundColor = color;
			break;
		default:
			break;
		}
	}

	void ClearBackground()
	{
		CloudAssert(_hasBeenInitialized, "Renderer::ClearBackground", "has not been initialized");
		switch (currentGraphicsAPI)
		{
		case GraphicsAPI::OpenGL:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;
		default:
			break;
		}
	}
}