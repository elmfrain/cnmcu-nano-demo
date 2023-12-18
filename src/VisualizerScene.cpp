#include <VisualizerScene.hpp>

#include <GLInclude.hpp>
#include <MeshBuilder.hpp>
#include <Visualizer.hpp>
#include <MeshObject.hpp>
// #include <iostream>

using namespace em;

static VisualizerScene* host = nullptr;

static void moveCamera(Camera& camera, float dt)
{ 
    const float speed = 5.0f;

    const Input& input = VisualizerApp::getInstance().getInput();

    if(input.isKeyHeld(GLFW_KEY_W))
    {
        float movex = -glm::sin(camera.getTransform().rotationEuler.y) * speed * dt;
        float movez = glm::cos(camera.getTransform().rotationEuler.y) * speed * dt;

        camera.getTransform().position += glm::vec3(movex, 0.0f, movez);
    }

    if(input.isKeyHeld(GLFW_KEY_S))
    {
        float movex = -glm::sin(camera.getTransform().rotationEuler.y) * speed * dt;
        float movez = glm::cos(camera.getTransform().rotationEuler.y) * speed * dt;

        camera.getTransform().position -= glm::vec3(movex, 0.0f, movez);
    }

    if(input.isKeyHeld(GLFW_KEY_A))
    {
        float movex = glm::cos(camera.getTransform().rotationEuler.y) * speed * dt;
        float movez = glm::sin(camera.getTransform().rotationEuler.y) * speed * dt;

        camera.getTransform().position -= glm::vec3(movex, 0.0f, movez);
    }

    if(input.isKeyHeld(GLFW_KEY_D))
    {
        float movex = glm::cos(camera.getTransform().rotationEuler.y) * speed * dt;
        float movez = glm::sin(camera.getTransform().rotationEuler.y) * speed * dt;

        camera.getTransform().position += glm::vec3(movex, 0.0f, movez);
    }

    if(input.isKeyHeld(GLFW_KEY_SPACE))
    {
        camera.getTransform().position.y += speed * dt;
    }

    if(input.isKeyHeld(GLFW_KEY_LEFT_SHIFT))
    {
        camera.getTransform().position.y -= speed * dt;
    }

    // std::cout << camera.getTransform().position.x << " " << camera.getTransform().position.y << " " << camera.getTransform().position.z << std::endl;
}

static void rotateCamera(Camera& camera, float dt)
{
    static bool mouseIsLocked = false;

    const Input& input = VisualizerApp::getInstance().getInput();

    if(input.isKeyPressed(GLFW_KEY_TAB))
    {
        mouseIsLocked = !mouseIsLocked;
        input.setLockMouse(mouseIsLocked);
    }

    if(!mouseIsLocked)
        return;

    const float speed = 0.5f;

    glm::vec2 mouseDelta = input.getMouseDelta();

    camera.getTransform().rotationMode = Transform::EULER_XYZ;
    camera.getTransform().rotationEuler.x -= mouseDelta.y * speed * dt;
    camera.getTransform().rotationEuler.y -= mouseDelta.x * speed * dt;

    // std::cout << camera.getTransform().rotationEuler.x << " " << camera.getTransform().rotationEuler.y << " " << camera.getTransform().rotationEuler.z << std::endl;
}

VisualizerScene::VisualizerScene()
{
    mainCamera.setName("main-camera");
    logger = Logger("VisualizerScene");
    host = this;
}

VisualizerScene::~VisualizerScene()
{
}

void VisualizerScene::init()
{
    auto windowSize = VisualizerApp::getInstance().getWindowSize();
    framebuffer.init(windowSize.x, windowSize.y);

    phongShader.init();

    compositor.init();

    mainCamera.getTransform().position.z = -2.0f;

    initFromLua();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void VisualizerScene::update(float dt)
{
    moveCamera(mainCamera, dt);
    rotateCamera(mainCamera, dt);
}

void VisualizerScene::draw()
{
    glm::ivec2 windowSize = VisualizerApp::getInstance().getWindowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    phongShader.setProjectionMatrix(mainCamera.getViewProjectionMatrix());
    phongShader.setCameraPos(mainCamera.getTransform().position);
    phongShader.setLightCount(lights.size());
    int i = 0;
    for(auto it = lights.begin(); it != lights.end(); ++it)
    {
        if(i >= MAX_LIGHTS) break;
        phongShader.getLight(i).position = it->second->getTransform().position;
        phongShader.getLight(i++).setLight(it->second->getColor(), it->second->getIntensity());
    }

    framebuffer.clear();
    phongShader.use();

    for(auto& object : objects)
    {
        object.second->draw(phongShader);
    }

    framebuffer.unbind();

    compositor.blit(framebuffer);
}

void VisualizerScene::destroy()
{
    framebuffer.destroy();

    phongShader.destroy();

    compositor.destroy();

    destroyObjectsAndLights();
}

void VisualizerScene::onWindowResize(int width, int height)
{
    framebuffer.resize(width, height);
}

LightObject& VisualizerScene::createLight(const std::string& name)
{
    if(lights.size() == MAX_LIGHTS)
        logger.warnf("Light %s will be ignored. Maximum number of lights reached.", name.c_str());


    lights[name] = std::make_unique<LightObject>(name);
    return static_cast<LightObject&>(lights[name].get()[0]);
}

LightObject& VisualizerScene::getLight(const std::string& name)
{
    return static_cast<LightObject&>(lights[name].get()[0]);
}

void VisualizerScene::initFromLua()
{
    // Create a new Lua state
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lua_openSceneLib(L);

    // Load and execute the Lua script
    int result = luaL_dofile(L, "res/scene.lua");

    // Check for errors
    if (result != LUA_OK)
    {
        const char* errorMsg = lua_tostring(L, -1);
        logger.errorf("Failed to run Lua script: %s", errorMsg);
    }

    logger.infof("Lua script executed successfully");

    lua_getglobal(L, "Objects");
    if(lua_istable(L, -1)) {
        lua_pushnil(L);
        int nObjs = 0;
        while(lua_next(L, -2) != 0) {
            nObjs++;
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
        logger.infof("There are %d objects", nObjs);
    }

    // Close the Lua state
    lua_close(L);
}

void VisualizerScene::destroyObjectsAndLights()
{
    objects.clear();
    lights.clear();
}

// Lua scene library functions

int VisualizerScene::lua_openSceneLib(lua_State* L)
{
    static const luaL_Reg sceneLib[] =
    {
        {"getHost", lua_getHost},
        {"createLight", lua_createLight},
        {"createObject", lua_createObject},
        {nullptr, nullptr}
    };

    luaL_newlib(L, sceneLib);
    lua_setglobal(L, "scene");

    return 0;
}

int VisualizerScene::lua_getHost(lua_State* L)
{
    lua_pushlightuserdata(L, host);

    return 1;
}

int VisualizerScene::lua_createLight(lua_State* L)
{
    int numArgs = lua_gettop(L);
    if (numArgs != 4)
        return luaL_error(L, "Expected 4 arguments, got %d", numArgs);

    const char* name = nullptr;
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    luaGet(name, const char*, string, 1);
    luaGetVec3(position, 2);
    luaGetVec3(color, 3);
    luaGet(intensity, float, number, 4);

    LightObject& light = host->createLight(name);
    light.getTransform().position = position;
    light.setColor(color);
    light.setIntensity(intensity);

    lua_pushlightuserdata(L, &light);

    return 1;
}

int VisualizerScene::lua_createObject(lua_State* L)
{
    int error = 0;
    int numArgs = lua_gettop(L);
    if (numArgs != 3)
        return luaL_error(L, "Expected 3 arguments, got %d", numArgs);

    const char* name = nullptr;
    const char* meshPath = nullptr;
    PhongMaterial material;

    luaGet(name, const char*, string, 1);
    luaGet(meshPath, const char*, string, 2);
    if((error = PhongMaterial::fromLua(L, 3, material)) != 0)
        return error;

    VertexFormat vtxFmt; 

    vtxFmt.size = 3;
    vtxFmt[0].data = EMVF_ATTRB_USAGE_POS |
                     EMVF_ATTRB_TYPE_FLOAT |
                     EMVF_ATTRB_SIZE(3) |
                     EMVF_ATTRB_NORMALIZED_FALSE;
    vtxFmt[1].data = EMVF_ATTRB_USAGE_UV |
                     EMVF_ATTRB_TYPE_FLOAT |
                     EMVF_ATTRB_SIZE(2) |
                     EMVF_ATTRB_NORMALIZED_FALSE;
    vtxFmt[2].data = EMVF_ATTRB_USAGE_NORMAL |
                     EMVF_ATTRB_TYPE_FLOAT |
                     EMVF_ATTRB_SIZE(3) |
                     EMVF_ATTRB_NORMALIZED_FALSE;

    MeshObject& object = host->createObject<MeshObject>(name);
    Mesh::Ptr mesh = Mesh::load(meshPath)[0];
    mesh->makeRenderable(vtxFmt);
    object.setMesh(mesh);
    object.setMaterial(material);

    lua_pushlightuserdata(L, &object);

    return 1;
}