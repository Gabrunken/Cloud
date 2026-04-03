#pragma once
#include <core/utils.hpp>
#include <subsystems/render_interface.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

	void InitializeWithoutEditor()
	{
		CloudAssert(!_hasBeenInitialized, "Game::InitializeWithoutEditor", "game already initialized");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

		window = glfwCreateWindow(windowWidth, windowHeight, "Game", NULL, NULL);
		CloudAssert(window, "Game::InitializeWithoutEditor", "window creation failed");

		glfwSetKeyCallback(window, KeyCallback);

		_hasBeenInitialized = true;
	}

	bool RunWithoutEditor()
	{
		CloudAssert(_hasBeenInitialized, "Game::RunWithoutEditor", "game has not been initialized");

		glfwPollEvents();
		return !glfwWindowShouldClose(window);
	}

	void TerminateWithoutEditor()
	{
		CloudAssert(_hasBeenInitialized, "Game::TerminateWithoutEditor", "game has not been initialized");
	}
#endif
}