#include <core/utils.hpp>
#include <subsystems/render_interface.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/application.hpp>
#include <math.h>
#include <core/window.hpp>
#include <tracy/Tracy.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

static bool _hasBeenInitialized = false;

static GLFWwindow* window;
static RenderInterface* renderer;

//Some hard-coded values
constexpr uint16_t windowWidth = 800, windowHeight = 600;

static const StartupSettings* settings;

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

	static void ImGuiPass()
	{
		switch (settings->graphicsAPI)
		{
		case GraphicsAPI::OpenGL:
			ImGui_ImplOpenGL3_NewFrame();
			break;
		case GraphicsAPI::Vulkan:
			break;
		case GraphicsAPI::DirectX12:
			break;
		case GraphicsAPI::Metal:
			break;
		default:
			return;
		}

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

#pragma region Test menu
		ImGui::Begin("Test menu");
		ImGui::Text("Statistics");
		ImGui::Text("Performance: %.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		static Color bgCol;
		if (ImGui::ColorEdit4("Clear color", (float*)&bgCol))
		{
			renderer->SetBackgroundColor(bgCol);
		}
		
		static bool windowDecorations = true;
		if (ImGui::Checkbox("Window decorations", &windowDecorations))
		{
			glfwSetWindowAttrib(window, GLFW_DECORATED, windowDecorations);
		}
		
		ImGui::End();
#pragma endregion

		ImGui::Render();

		switch (settings->graphicsAPI)
		{
		case GraphicsAPI::OpenGL:
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			break;
		case GraphicsAPI::Vulkan:
			break;
		case GraphicsAPI::DirectX12:
			break;
		case GraphicsAPI::Metal:
			break;
		default:
			return;
		}
	}

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{

	}

	static void GLFWErrorCallback(int error, const char* description)
	{
		Logger::PushMessage("Game::GLFWErrorCallback", description, Logger::Error);
	}

	void InitializeWithoutEditor()
	{
		CloudAssert(!_hasBeenInitialized, "Game::InitializeWithoutEditor", "game already initialized");

		settings = &Application::GetSettings();

		Window::RenameWindow("no title", "Game");
		window = Window::GetWindow("Game");
		glfwSetWindowSize(window, 1000, 600);

		ImGui_ImplGlfw_InitForOther(window, true);

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetErrorCallback(GLFWErrorCallback);

		renderer = &Renderer::GetRenderer();
		renderer->SetBackgroundColor({1.001 /*Gets fully transparent otherwise...*/, 1.0, 1.0, 1.0});

		_hasBeenInitialized = true;
	}

	bool RunWithoutEditor()
	{
		ZoneScopedN("Main Loop")
		CloudAssert(_hasBeenInitialized, "Game::RunWithoutEditor", "game has not been initialized");

		double time = glfwGetTime();

		renderer->ClearBackground();
		
		ImGuiPass();

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
