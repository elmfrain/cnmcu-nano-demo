#pragma once

#include <SceneObject.hpp>

namespace em 
{
    class Camera : public SceneObject
    {
    public:
        enum ProjectionMode
        {
            PERSPECTIVE = 0,
            ORTHOGRAPHIC
        };
        static const char* const ProjectionModeNames[];
        enum FOVMode
        {
            VERTICAL = 0,
            HORIZONTAL
        };
        static const char* const FOVModeNames[];

        Camera(const std::string& name = "Camera");
        virtual ~Camera();

        virtual void draw(Shader& shader) override;

        ProjectionMode projectionMode;
        FOVMode fovMode;

        float orthographicSize;
        float fieldOfView;
        float nearPlane;
        float farPlane;

        glm::vec2 perspectiveShift;

        const glm::mat4 getProjectionMatrix() const;
        const glm::mat4 getViewMatrix() const;
        const glm::mat4 getViewProjectionMatrix() const;

        int lua_this(lua_State* L) override;
        static int lua_openCameraLib(lua_State* L);
    private:
        static int lua_getProjectionMode(lua_State* L);
        static int lua_getFOVMode(lua_State* L);
        static int lua_getOrthographicSize(lua_State* L);
        static int lua_getFieldOfView(lua_State* L);
        static int lua_getNearPlane(lua_State* L);
        static int lua_getFarPlane(lua_State* L);
        static int lua_getPerspectiveShift(lua_State* L);

        static int lua_setProjectionMode(lua_State* L);
        static int lua_setFOVMode(lua_State* L);
        static int lua_setOrthographicSize(lua_State* L);
        static int lua_setFieldOfView(lua_State* L);
        static int lua_setNearPlane(lua_State* L);
        static int lua_setFarPlane(lua_State* L);
        static int lua_setPerspectiveShift(lua_State* L);
    protected:
        virtual void update(float dt) override;
    };
}