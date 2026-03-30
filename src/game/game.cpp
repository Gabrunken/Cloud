#pragma once
#include <core/utils.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static bool _hasBeenInitialized = false;

static GLFWwindow* window;

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

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); //MacOS requires this
#endif
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

		window = glfwCreateWindow(windowWidth, windowHeight, "Game", NULL, NULL);
		CloudAssert(window, "Game::InitializeWithoutEditor", "window creation failed");

		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		glfwSetKeyCallback(window, KeyCallback);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glEnable(GL_DEPTH_TEST);

		_hasBeenInitialized = true;
	}

	bool RunWithoutEditor()
	{
		CloudAssert(_hasBeenInitialized, "Game::RunWithoutEditor", "game has not been initialized");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
		return !glfwWindowShouldClose(window);
	}

	void TerminateWithoutEditor()
	{
		CloudAssert(_hasBeenInitialized, "Game::TerminateWithoutEditor", "game has not been initialized");
	}
#endif
}