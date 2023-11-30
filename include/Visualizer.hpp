#pragma once

#include <Logger.hpp>
#include <GLFWInclude.hpp>
#include <inttypes.h>

namespace em {
    struct AppOptions
    {
        uint8_t msaaSamples = 2;
        uint16_t width = 800;
        uint16_t height = 600;
        uint16_t minWidth = 480;
        uint16_t minHeight = 320;
        bool fullscreen = false;
        bool vsync = true;
        bool resizable = true;
    };

    class VisualizerApp 
    {
    public:
        VisualizerApp();

        bool start(AppOptions& settings);
        bool shouldClose();
        bool runLoop();
        bool terminate();

        void setFullscreen(bool fullscreen);

        AppOptions getOptions();
    private:
        Logger m_logger;
        AppOptions m_options;
        GLFWwindow* m_window;
        bool m_fullscreen;
        bool m_shouldClose;
        bool m_initialized;
    };
}