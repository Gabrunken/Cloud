#pragma once
#include <core/graphics_api.hpp>
#include <string>

enum class UIStyle
{
    ImGuiClassic,
    ImGuiDark,
    ImGuiLight
};

struct StartupSettings
{
public:
    std::string loggerFilePath = "log.txt";
    GraphicsAPI graphicsAPI = GraphicsAPI::OpenGL;
    uint8_t openGLMajorVersionRequired = 3, openGLMinorVersionRequired = 3;
    UIStyle imGuiStyle = UIStyle::ImGuiClassic;

    StartupSettings() = default;
};