#pragma once
#include <core/color.hpp>

enum class GraphicsAPI
{
	OpenGL,
	Vulkan,
	DirectX12,
	Metal
};

struct RenderingContext
{
	Color backgroundColor;
};

/*
* This should only be initialized one time and left as it is for the whole runtime.
* To make interchangables graphics apis, i should probably make a dll for each api, and link it at runtime.
* Otherwise, like this i have the little overhead of a switch, each api call.
*/
static GraphicsAPI currentGraphicsAPI;
static RenderingContext currentRenderingContext;

namespace Renderer
{
	bool Initialize(GraphicsAPI graphicsAPI);
	void Terminate();
	void SetBackgroundColor(const Color& color);
	void ClearBackground();
}