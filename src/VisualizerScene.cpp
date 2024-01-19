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

    mainCamera = std::make_unique<Camera>();
    mainCamera->setName("main-camera");
    mainCamera->getTransform().position.z = -2.0f;

    initLua();
    initFromLua();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void VisualizerScene::reload()
{
    logger.infof("Reloading scene");

    destroyObjectsAndLights();
    destroyLua();
    initLua();
    initFromLua();
}

void VisualizerScene::update(float dt)
{
    moveCamera(*mainCamera, dt);
    rotateCamera(*mainCamera, dt);
    updateFromLua(dt);

    const Input& input = VisualizerApp::getInstance().getInput();

    if(input.isKeyPressed(GLFW_KEY_R))
        reload();
        
    for(auto&lights : lights)
        lights.second->doUpdate(dt);

    for(auto& object : objects)
        object.second->doUpdate(dt);

    mainCamera->doUpdate(dt);
}

void VisualizerScene::draw()
{
    glm::ivec2 windowSize = VisualizerApp::getInstance().getWindowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    phongShader.setProjectionMatrix(mainCamera->getViewProjectionMatrix());
    phongShader.setCameraPos(mainCamera->getTransform().position);
    phongShader.setLightCount(lights.size());
    lightIndex = 0;

    framebuffer.clear();
    phongShader.use();

    for(auto& light : lights)
        if(light.second->isTopLevel())
            drawObject(light.second.get(), glm::mat4(1.0f));

    for(auto& object : objects)
        if(object.second->isTopLevel())
            drawObject(object.second.get(), glm::mat4(1.0f));

    framebuffer.unbind();

    compositor.blit(framebuffer);
}

void VisualizerScene::destroy()
{
    framebuffer.destroy();

    phongShader.destroy();

    compositor.destroy();

    mainCamera.reset();

    destroyObjectsAndLights();

    destroyLua();
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

void VisualizerScene::drawObject(SceneObject* object, glm::mat4 modelView)
{
    modelView *= object->getConstTransform().getMatrix();

    if(object->getType() == SceneObject::MESH)
    {
        MeshObject* meshObject = static_cast<MeshObject*>(object);

        phongShader.setModelViewMatrix(modelView);
        meshObject->draw(phongShader);
    }
    else if(object->getType() == SceneObject::LIGHT && lightIndex < MAX_LIGHTS)
    {
        LightObject* lightObject = static_cast<LightObject*>(object);
        glm::vec4 lightPos = modelView * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        phongShader.getLight(lightIndex).position = lightPos;
        phongShader.getLight(lightIndex++).setLight(lightObject->getColor(), lightObject->getIntensity());
    }

    for(auto& child : object->getChildren())
        drawObject(child, modelView);
}

void VisualizerScene::initLua()
{
    // Create a new Lua state
    L = luaL_newstate();
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

    lua_getglobal(L, "Start");
    if(!lua_isfunction(L, -1))
        logger.warnf("Lua script does not have a Start function");
    lua_pop(L, 1);

    lua_getglobal(L, "Update");
    if(!lua_isfunction(L, -1))
        logger.warnf("Lua script does not have an Update function");
    lua_pop(L, 1);

    logger.infof("Lua script executed successfully");
}

void VisualizerScene::initFromLua()
{
    lua_getglobal(L, "Start");
    if(!lua_isfunction(L, -1))
        return lua_pop(L, 1);

    lua_call(L, 0, 0);
}

void VisualizerScene::updateFromLua(float dt)
{
    lua_getglobal(L, "Update");
    if(!lua_isfunction(L, -1))
        return lua_pop(L, 1);

    lua_pushnumber(L, dt);
    lua_call(L, 1, 0);
}

void VisualizerScene::destroyObjectsAndLights()
{
    objects.clear();
    lights.clear();
}

void VisualizerScene::destroyLua()
{
    lua_close(L);
}

// Lua scene library functions

int VisualizerScene::lua_openSceneLib(lua_State* L)
{
    static const luaL_Reg sceneLib[] =
    {
        {"getHost", lua_getHost},
        {"createLight", lua_createLight},
        {"createObject", lua_createObject},
        {"loadMeshes", lua_loadMeshes},
        {nullptr, nullptr}
    };

    Compositor::lua_openCompositorLib(L);
    SceneObject::lua_openSceneObjectLib(L);

    luaL_newlib(L, sceneLib);
    lua_setglobal(L, "scene");

    lua_getglobal(L, "scene");
    lua_pushstring(L, "compositor");
    host->compositor.lua_this(L);
    lua_settable(L, -3);
    lua_pushstring(L, "camera");
    host->mainCamera->lua_this(L);
    lua_settable(L, -3);
    lua_pop(L, 1);

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
    if (numArgs != 1)
        return luaL_error(L, "Expected 1 argument, got %d", numArgs);

    const char* name = nullptr;

    luaGet(name, const char*, string, 1);

    LightObject& light = host->createLight(name);

    light.lua_this(L);

    return 1;
}

int VisualizerScene::lua_createObject(lua_State* L)
{
    int numArgs = lua_gettop(L);
    if (numArgs != 1)
        return luaL_error(L, "Expected 1 arguments, got %d", numArgs);

    const char* name = nullptr;

    luaGet(name, const char*, string, 1);

    MeshObject& object = host->createObject<MeshObject>(name);

    object.lua_this(L);

    return 1;
}

int VisualizerScene::lua_loadMeshes(lua_State* L)
{
    static VertexFormat vtxFmt; 

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

    int numArgs = lua_gettop(L);
    if (numArgs != 1)
        return luaL_error(L, "Expected 1 argument, got %d", numArgs);

    const char* path = nullptr;

    luaGet(path, const char*, string, 1);

    std::vector<Mesh::Ptr> meshes = Mesh::load(path);

    lua_newtable(L);
    for(int i = 0; i < meshes.size(); ++i)
    {
        lua_newtable(L);
        lua_pushstring(L, "ptr");
        luaPushSharedPtr<Mesh>(L, meshes[i], "MeshPtr");
        lua_settable(L, -3);

        luaL_newmetatable(L, "Mesh");
        lua_pushstring(L, "makeRenderable");
        lua_pushcfunction(L, [](lua_State* L) -> int
        {
            luaPushValueFromKey("ptr", 1);
            Mesh::Ptr mesh;
            luaGetSharedPtr<Mesh>(L, mesh, "MeshPtr", -1);
            mesh->makeRenderable(vtxFmt);
            return 0;
        });
        lua_settable(L, -3);
        lua_pushstring(L, "getName");
        lua_pushcfunction(L, [](lua_State* L) -> int
        {
            luaPushValueFromKey("ptr", 1);
            Mesh::Ptr mesh;
            luaGetSharedPtr<Mesh>(L, mesh, "MeshPtr", -1);
            lua_pushstring(L, mesh->getName());
            return 1;
        });
        lua_settable(L, -3);
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2); 
        lua_settable(L, -3);

        lua_setmetatable(L, -2);
        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}