#include <core/utils.hpp>
#include <subsystems/render_interface.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/application.hpp>
#include <math.h>
#include <core/window.hpp>

static bool _hasBeenInitialized = false;

static GLFWwindow* window;
static RenderInterface* renderer;

//Some hard-coded values
constexpr uint16_t windowWidth = 800, windowHeight = 600;

namespace Game
{
#ifdef CLOUD_EDITOR
	void Initialize()
	{
		return;
	}

	bool Run()
	{
		return false;
	}

	void Terminate()
	{
		return;
	}
#else

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{

	}

	void GLFWErrorCallback(int error, const char* description)
	{
		Logger::PushMessage("Game::GLFWErrorCallback", description, Logger::Error);
	}

	void InitializeWithoutEditor()
	{
		CloudAssert(!_hasBeenInitialized, "Game::InitializeWithoutEditor", "game already initialized");

		Window::RenameWindow("no title", "Game");
		window = Window::GetWindow("Game");
		glfwSetWindowSize(window, 1000, 600);

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetErrorCallback(GLFWErrorCallback);

		renderer = &Renderer::GetRenderer();
		renderer->SetBackgroundColor({1.0f, 1.0f, 1.0f, 1.0f});

		_hasBeenInitialized = true;
	}

	bool RunWithoutEditor()
	{
		CloudAssert(_hasBeenInitialized, "Game::RunWithoutEditor", "game has not been initialized");

		double time = glfwGetTime();

		renderer->ClearBackground();
		
		if (Application::GetSettings().graphicsAPI == GraphicsAPI::OpenGL) { glfwSwapBuffers(window); }
		else { renderer->Present(); }
		
		glfwPollEvents();
		return !glfwWindowShouldClose(window);
	}

	void TerminateWithoutEditor()
	{
		CloudAssert(_hasBeenInitialized, "Game::TerminateWithoutEditor", "game has not been initialized");
	}
#endif
}
