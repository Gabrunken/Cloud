#pragma once
#include <core/graphics_api.hpp>
#include <string>
#include <stdint.h>

#define SETTINGS_FILE_PATH "settings.txt"

//If some properties are not specified in the settings file, these ones are used.
#define DEFAULT_LOG_FILE_PATH "log.txt"
#define DEFAULT_GRAPHICS_API GraphicsAPI::OpenGL
//This is the DEFAULT REQUESTED OpenGL version, an higher version might be chosen by glfw depending on the implementation.
#define DEFAULT_OPENGL_MAJOR 3
#define DEFAULT_OPENGL_MINOR 3
#define DEFAULT_UI_STYLE UIStyle::ImGuiClassic

enum class UIStyle
{
    ImGuiClassic,
    ImGuiDark,
    ImGuiLight
};

struct StartupSettings
{
public:
    std::string loggerFilePath = DEFAULT_LOG_FILE_PATH;
    GraphicsAPI graphicsAPI = DEFAULT_GRAPHICS_API;
    //Note: this is the version requested, not necessarily the used one. A fallback to an older version might've happened.
    uint8_t openGLMajorVersionRequested = DEFAULT_OPENGL_MAJOR, openGLMinorVersionRequested = DEFAULT_OPENGL_MINOR;
    UIStyle imGuiStyle = DEFAULT_UI_STYLE;

    StartupSettings() = default;
};