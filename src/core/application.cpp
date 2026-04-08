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
        FILE* settingsFile = fopen(settingsFilePath, "a+");
        if (!settingsFile)
        {
            #ifdef SHOW_CONSOLE
            printf("Failed to open the settings file at %s\n", settingsFilePath);
            #endif
            return false;
        }

        //Read through properties, if a property is not found, or is incorrectly named, the default one will be used instead, defined in the StartupSettings constructor
        while (true)
        {
            char property[100] = { 0 };
            char value[100] = { 0 };
            if (fscanf(settingsFile, "%s - %s\n", property, value) == EOF)
                break;

            size_t valueLen = strlen(value);
            if (value[valueLen - 1] == '\n')
                value[valueLen - 1] = '\0';

            if (strcmp(property, "LoggerFilePath") == 0)
            {
                _settings.loggerFilePath = value;
            }

            else if (strcmp(property, "GraphicsAPI") == 0)
            {
                if (strcmp(value, "OpenGL") == 0)
                { _settings.graphicsAPI = GraphicsAPI::OpenGL; }
                else if (strcmp(value, "Vulkan") == 0)
                { _settings.graphicsAPI = GraphicsAPI::Vulkan; }
                else if (strcmp(value, "DirectX12") == 0)
                { _settings.graphicsAPI = GraphicsAPI::DirectX12; }
                else if (strcmp(value, "Metal") == 0)
                { _settings.graphicsAPI = GraphicsAPI::Metal; }
            }

            else if (strcmp(property, "OpenGL_Required") == 0)
            {
                _settings.openGLMajorVersionRequired = value[0] - '0';
                _settings.openGLMinorVersionRequired = value[2] - '0';
            }

            else if (strcmp(property, "UIStyle") == 0)
            {
                if (strcmp(value, "ImGuiClassic") == 0)
                {
                    _settings.imGuiStyle = UIStyle::ImGuiClassic;
                }

                else if (strcmp(value, "ImGuiDark") == 0)
                {
                    _settings.imGuiStyle = UIStyle::ImGuiDark;
                }

                else if (strcmp(value, "ImGuiLight") == 0)
                {
                    _settings.imGuiStyle = UIStyle::ImGuiLight;
                }
            }
        }

        #ifdef __APPLE__
        if (_settings.graphicsAPI != GraphicsAPI::Metal)
            #ifdef SHOW_CONSOLE
            printf("On MacOS, Metal graphics API is required.\n");
            #endif
            return false;
        #elif __linux__
        if (_settings.graphicsAPI == GraphicsAPI::DirectX12)
            #ifdef SHOW_CONSOLE
            printf("On linux, DirectX12 graphics API is not supported.\n");
            #endif
            return false;
        #endif
        
        fclose(settingsFile);

        /*
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
        */

        _initialized = true;
        return true;
    }

    const StartupSettings& GetSettings()
    {
        assert(_initialized);

        return _settings;
    }
}