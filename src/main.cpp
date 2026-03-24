#include <iostream>
#include <third-party/GLFW/glfw3.h>

int main(int argc, char** argv)
{
    if (!glfwInit())
    {
        printf("Failed to initialize glfw.\n");
    }

    std::printf("Hello, Cloud!\n");

    glfwTerminate();
    return 0;
}