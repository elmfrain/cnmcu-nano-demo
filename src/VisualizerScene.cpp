#include <VisualizerScene.hpp>

#include <GLInclude.hpp>
#include <MeshBuilder.hpp>
#include <Visualizer.hpp>
#include <MeshObject.hpp>
// #include <iostream>

using namespace em;

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
}

VisualizerScene::~VisualizerScene()
{
}

void VisualizerScene::init()
{
    auto windowSize = VisualizerApp::getInstance().getWindowSize();
    framebuffer.init(windowSize.x, windowSize.y);

    phongShader.init();
    PhongMaterial material;
    material.diffuse = glm::vec3(0.0f);
    material.specular = glm::vec3(1.0f);
    material.roughnessToShininess(0.23f);
    phongShader.setMaterial(material);

    compositor.init();

    mainCamera.getTransform().position.z = -2.0f;

    initObjects();
    initLights();

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

void VisualizerScene::initObjects()
{
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

    MeshObject& sare = createObject<MeshObject>("sare");
    Mesh::Ptr sareMesh = Mesh::load("res/sare.obj")[0];
    sareMesh->makeRenderable(vtxFmt);
    sare.setMesh(sareMesh);

    MeshObject& planet = createObject<MeshObject>("planet");
    Mesh::Ptr planetMesh = Mesh::load("res/planet.obj")[0];
    planetMesh->makeRenderable(vtxFmt);
    planet.setMesh(planetMesh);

    MeshObject& ring = createObject<MeshObject>("ring");
    Mesh::Ptr ringMesh = Mesh::load("res/ring.obj")[0];
    ringMesh->makeRenderable(vtxFmt);
    ring.setMesh(ringMesh);

    MeshObject& backdrop = createObject<MeshObject>("backdrop");
    Mesh::Ptr backdropMesh = Mesh::load("res/backdrop.obj")[0];
    backdropMesh->makeRenderable(vtxFmt);
    
    backdrop.setMesh(backdropMesh);
}

void VisualizerScene::initLights()
{
    LightObject& cyan = createLight("cyan");
    cyan.getTransform().position = glm::vec3(-3.0f, 5.0f, 4.0f);
    cyan.setColor(glm::vec3(0.0f, 0.86f, 1.0f));
    cyan.setIntensity(3.0f);

    LightObject& magenta = createLight("magenta");
    magenta.getTransform().position = glm::vec3(3.0f, 5.0f, -4.0f);
    magenta.setColor(glm::vec3(1.0f, 0.0f, 0.77f));
    magenta.setIntensity(3.0f);

    LightObject& fore = createLight("fore");
    fore.getTransform().position = glm::vec3(0.0f, 4.0f, 0.0f);
    fore.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    fore.setIntensity(0.1f);
}

void VisualizerScene::destroyObjectsAndLights()
{
    objects.clear();
    lights.clear();
}