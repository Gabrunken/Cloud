#include <core/window.hpp>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <string>
#include <backends/imgui_impl_glfw.h>

static std::unordered_map<std::string, GLFWwindow*> _windows;

namespace Window
{
    GLFWwindow* CreateWindow(const char* title, uint16_t width, uint16_t height)
    {
        GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) return nullptr;
        _windows[title] = window;
        return window;
    }

    GLFWwindow* GetWindow(const char* title)
    {
        return _windows.at(title);
    }

    void DestroyWindow(const char* title)
    {
        GLFWwindow* window = _windows.at(title);
        if (window)
        {
            glfwDestroyWindow(window);
        }
    }

    void RenameWindow(const char* oldTitle, const char* newTitle)
    {
        auto node = _windows.extract(oldTitle);

        if (!node.empty())
        {
            node.key() = newTitle;

            _windows.insert(std::move(node));
            glfwSetWindowTitle(_windows[newTitle], newTitle);
        }
    }
}