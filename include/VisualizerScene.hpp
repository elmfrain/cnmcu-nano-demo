#pragma once

#include <Camera.hpp>
#include <shaders/Shader.hpp>
#include <unordered_map>
#include <string>
#include <memory>

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

        std::unordered_map<std::string, std::unique_ptr<SceneObject>> objects;

        template <typename T>
        T& createObject(const std::string& name)
        {
            objects[name] = std::make_unique<T>(name);
            return static_cast<T&>(objects[name].get()[0]);
        }

        template <typename T>
        T getObject(const std::string& name)
        {
            return static_cast<T>(objects[name]);
        }

        void initObjects();
        void destroyObjects();
    };
}