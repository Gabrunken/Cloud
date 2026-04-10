#pragma once
#include <stdint.h>

typedef struct GLFWwindow GLFWwindow;

namespace Window
{
    //Creates a GLFW window, set the hints manually
    GLFWwindow* CreateWindow(const char* title, uint16_t width, uint16_t height);
    GLFWwindow* GetWindow(const char* title);
    void DestroyWindow(const char* title);
    void RenameWindow(const char* oldTitle, const char* newTitle);
}