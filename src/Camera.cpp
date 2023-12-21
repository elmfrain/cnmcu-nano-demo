#include <Camera.hpp>
#include <Visualizer.hpp>

using namespace em;

const char* const Camera::ProjectionModeNames[] = { "Perspective", "Orthographic" };
const char* const Camera::FOVModeNames[] = { "Vertical", "Horizontal" };

Camera::Camera(const std::string& name)
    : SceneObject(CAMERA, name)
    , projectionMode(PERSPECTIVE)
    , fovMode(VERTICAL)
    , orthographicSize(1.0f)
    , fieldOfView(45.0f)
    , nearPlane(0.1f)
    , farPlane(100.0f)
    , perspectiveShift(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::update(float dt)
{
}

void Camera::draw(Shader& shader)
{
}

const glm::mat4 Camera::getProjectionMatrix() const
{
    glm::mat4 projectionMatrix(1.0f);

    glm::vec2 windowSize = VisualizerApp::getInstance().getWindowSize();
    
    if(projectionMode == PERSPECTIVE)
    {
        float aspectRatio = windowSize.x / windowSize.y;
        float fovRadians = fovMode == VERTICAL ? glm::radians(fieldOfView) : 
            2.0f * glm::atan(glm::tan(glm::radians(fieldOfView) / 2.0f) * aspectRatio);
        projectionMatrix = glm::translate(projectionMatrix, glm::vec3(perspectiveShift, 0.0f));
        projectionMatrix = projectionMatrix * glm::perspective(fovRadians, aspectRatio, nearPlane, farPlane);
    }
    else
    {
        float halfSize = orthographicSize / 2.0f;
        float aspectRatio = fovMode == VERTICAL ? windowSize.x / windowSize.y : windowSize.y / windowSize.x;
        float xSize = fovMode == HORIZONTAL ? halfSize : halfSize / aspectRatio;
        float ySize = fovMode == VERTICAL ? halfSize : halfSize / aspectRatio;
        projectionMatrix = glm::ortho(-xSize, xSize, -ySize, ySize, nearPlane, farPlane);
    }

    return projectionMatrix;
}

const glm::mat4 Camera::getViewMatrix() const
{
    return getConstTransform().getInverseMatrix();
}

const glm::mat4 Camera::getViewProjectionMatrix() const
{
    return getProjectionMatrix() * getViewMatrix();
}

#define luaGetCamera() \
    Camera* camera; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(camera, Camera, -1);

int Camera::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);
    lua_pushstring(L, "transform");
    getTransform().lua_this(L);
    lua_settable(L, -3);
    
    luaL_newmetatable(L, "Camera");
    lua_setmetatable(L, -2);

    return 1;
}

int Camera::lua_openCameraLib(lua_State* L)
{
    static const luaL_Reg luaCameraMethods[] = {
        { "getProjectionMode", lua_getProjectionMode },
        { "getFOVMode", lua_getFOVMode },
        { "getOrthographicSize", lua_getOrthographicSize },
        { "getFieldOfView", lua_getFieldOfView },
        { "getNearPlane", lua_getNearPlane },
        { "getFarPlane", lua_getFarPlane },
        { "getPerspectiveShift", lua_getPerspectiveShift },
        { "setProjectionMode", lua_setProjectionMode },
        { "setFOVMode", lua_setFOVMode },
        { "setOrthographicSize", lua_setOrthographicSize },
        { "setFieldOfView", lua_setFieldOfView },
        { "setNearPlane", lua_setNearPlane },
        { "setFarPlane", lua_setFarPlane },
        { "setPerspectiveShift", lua_setPerspectiveShift },
        { nullptr, nullptr }
    };

    luaL_newmetatable(L, "Camera");
    luaL_setfuncs(L, luaCameraMethods, 0);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    luaL_newmetatable(L, "SceneObject");
    lua_setmetatable(L, -2);

    lua_setglobal(L, "Camera");

    return 0;
}

int Camera::lua_getProjectionMode(lua_State* L)
{
    luaGetCamera();
    lua_pushstring(L, ProjectionModeNames[camera->projectionMode]);

    return 1;
}

int Camera::lua_getFOVMode(lua_State* L)
{
    luaGetCamera();
    lua_pushstring(L, FOVModeNames[camera->fovMode]);

    return 1;
}

int Camera::lua_getOrthographicSize(lua_State* L)
{
    luaGetCamera();
    lua_pushnumber(L, camera->orthographicSize);

    return 1;
}

int Camera::lua_getFieldOfView(lua_State* L)
{
    luaGetCamera();
    lua_pushnumber(L, camera->fieldOfView);

    return 1;
}

int Camera::lua_getNearPlane(lua_State* L)
{
    luaGetCamera();
    lua_pushnumber(L, camera->nearPlane);

    return 1;
}

int Camera::lua_getFarPlane(lua_State* L)
{
    luaGetCamera();
    lua_pushnumber(L, camera->farPlane);

    return 1;
}

int Camera::lua_getPerspectiveShift(lua_State* L)
{
    luaGetCamera();
    luaPushVec2(camera->perspectiveShift);

    return 2;
}

int Camera::lua_setProjectionMode(lua_State* L)
{
    luaGetCamera();
    camera->projectionMode = static_cast<ProjectionMode>(luaL_checkoption(L, 2, nullptr, ProjectionModeNames));

    return 0;
}

int Camera::lua_setFOVMode(lua_State* L)
{
    luaGetCamera();
    camera->fovMode = static_cast<FOVMode>(luaL_checkoption(L, 2, nullptr, FOVModeNames));

    return 0;
}

int Camera::lua_setOrthographicSize(lua_State* L)
{
    luaGetCamera();
    luaGet(camera->orthographicSize, float, number, 2);

    return 0;
}

int Camera::lua_setFieldOfView(lua_State* L)
{
    luaGetCamera();
    luaGet(camera->fieldOfView, float, number, 2);

    return 0;
}

int Camera::lua_setNearPlane(lua_State* L)
{
    luaGetCamera();
    luaGet(camera->nearPlane, float, number, 2);

    return 0;
}

int Camera::lua_setFarPlane(lua_State* L)
{
    luaGetCamera();
    luaGet(camera->farPlane, float, number, 2);

    return 0;
}

int Camera::lua_setPerspectiveShift(lua_State* L)
{
    luaGetCamera();
    luaGetVec2(camera->perspectiveShift, 2);

    return 0;
}