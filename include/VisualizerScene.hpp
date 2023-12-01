#pragma once

#include <Camera.hpp>

namespace em
{
    class VisualizerScene
    {
    public:
        VisualizerScene();
        ~VisualizerScene();

        void update(float dt);
        void draw();

    private:
        Camera mainCamera;
    };
}