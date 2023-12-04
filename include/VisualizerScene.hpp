#pragma once

#include <Camera.hpp>
#include <shaders/Shader.hpp>
#include <Mesh.hpp>

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

        Mesh::Ptr sareMesh;
        Mesh::Ptr planetMesh;

        void initMeshes();
        void destroyMeshes();
    };
}