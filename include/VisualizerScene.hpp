#pragma once

#include <Camera.hpp>
#include <LightObject.hpp>
#include <shaders/PhongShader.hpp>
#include <Framebuffer.hpp>
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

        void onWindowResize(int width, int height);

    private:
        Camera mainCamera;
        Framebuffer framebuffer;
        PhongShader phongShader;
        Logger logger;

        std::unordered_map<std::string, std::unique_ptr<SceneObject>> objects;
        std::unordered_map<std::string, std::unique_ptr<LightObject>> lights;

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

        LightObject& createLight(const std::string& name);
        LightObject& getLight(const std::string& name);

        void initObjects();
        void initLights();
        void destroyObjectsAndLights();
    };
}