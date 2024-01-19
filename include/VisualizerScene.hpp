#pragma once

#include <Camera.hpp>
#include <LightObject.hpp>
#include <shaders/PhongShader.hpp>
#include <Framebuffer.hpp>
#include <shaders/Compositor.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <LuaInclude.hpp>

namespace em
{
    class VisualizerScene
    {
    public:
        VisualizerScene();
        ~VisualizerScene();

        void init();
        void reload();
        void update(float dt);
        void draw();
        void destroy();

        void onWindowResize(int width, int height);

    private:
        std::unique_ptr<Camera> mainCamera;
        Framebuffer framebuffer;
        PhongShader phongShader;
        Compositor compositor;
        Logger logger;

        lua_State* L;

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

        int lightIndex;
        void drawObject(SceneObject* object, glm::mat4 modelView);

        void initLua();
        void initFromLua();
        void updateFromLua(float dt);
        void destroyObjectsAndLights();
        void destroyLua();

        static int lua_openSceneLib(lua_State* L);
        static int lua_getHost(lua_State* L);
        static int lua_createLight(lua_State* L);
        static int lua_createObject(lua_State* L);
        static int lua_loadMeshes(lua_State* L);
    };
}