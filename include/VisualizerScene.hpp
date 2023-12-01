#pragma once

#include <Camera.hpp>
#include <shaders/Shader.hpp>

namespace em
{
    class VisualizerScene
    {
    public:
        VisualizerScene();
        ~VisualizerScene();

        void init();
        void update(float dt);
        void draw();
        void destroy();

    private:
        Camera mainCamera;
        Shader basicShader;
    };
}