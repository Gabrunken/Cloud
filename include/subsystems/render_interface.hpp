#pragma once
#include <core/color.hpp>
#include <string>
#include <core/utils.hpp>

#ifdef BUILD_RENDERER
	#define RENDERER_API API_EXPORT
#else
	#define RENDERER_API API_IMPORT
#endif

enum class GraphicsAPI
{
	OpenGL,
	Vulkan,
	DirectX12,
	Metal
};

struct RenderContext
{
	Color backgroundColor;
};

class RenderInterface
{
public:
	virtual ~RenderInterface() = default;
	virtual void SetBackgroundColor(const Color& color) = 0;
	virtual void ClearBackground() = 0;
};

using LogCallback = void(*)(const std::string&, const std::string&, int);

//This function will be overridden by each Renderer API, returning the correct Renderer.
//Multiple renderers cannot be linked at the same runtime instance, an API change will
//require a restart.
using CreateRendererFunc = RenderInterface* (*)(LogCallback);
using DestroyRendererFunc = void (*)(RenderInterface*);

namespace Renderer
{
	bool Initialize(GraphicsAPI graphicsAPI);
	RenderInterface& GetRenderer();
	void Terminate();
}