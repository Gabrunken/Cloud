#include <core/application.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

static bool _initialized = false;
static StartupSettings _settings;

namespace Application
{
    bool Initialize(const char* settingsFilePath)
    {
        if (_initialized) return true;

        //Miscellaneous
        srand(time(nullptr));

        //Load settings file
        FILE* settings = fopen(settingsFilePath, "r");
        if (!settings) return false;

        char line[100] = {0};

        //First line is logger file path
        if (!fgets(line, sizeof(line), settings)) return false;

        line[strlen(line) - 1] = 0;
        _settings.loggerFilePath = line;

        //Second line is graphics API
        if (!fgets(line, sizeof(line), settings)) return false;
        if (strcmp(line, "OpenGL\n") == 0)
        {
            _settings.graphicsAPI = GraphicsAPI::OpenGL;
        }

        else if (strcmp(line, "DirectX12\n") == 0)
        {
            _settings.graphicsAPI = GraphicsAPI::DirectX12;
        }

        else if (strcmp(line, "Vulkan\n") == 0)
        {
            _settings.graphicsAPI = GraphicsAPI::Vulkan;
        }

        else if (strcmp(line, "Metal\n") == 0)
        {
            _settings.graphicsAPI = GraphicsAPI::Metal;
        }

        else { return false; }

        //Third line is opengl major and minor version, separated by '.'
        if (!fgets(line, sizeof(line), settings)) return false;
        _settings.openGLMajorVersionRequired = line[0] - '0';
        _settings.openGLMinorVersionRequired = line[2] - '0';

        _initialized = true;
        return true;
    }

    const StartupSettings& GetSettings()
    {
        assert(_initialized);

        return _settings;
    }
}