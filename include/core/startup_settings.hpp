#pragma once
#include <core/graphics_api.hpp>
#include <string>

struct StartupSettings
{
public:
    std::string loggerFilePath;
    GraphicsAPI graphicsAPI;
    uint8_t openGLMinorVersionRequired, openGLMajorVersionRequired;

    StartupSettings() = default;
};