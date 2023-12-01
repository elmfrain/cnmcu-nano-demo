#include <VisualizerScene.hpp>

#include <GLInclude.hpp>
#include <MeshBuilder.hpp>
#include <Visualizer.hpp>
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
}

VisualizerScene::~VisualizerScene()
{
}

void VisualizerScene::init()
{
    basicShader.init();

    mainCamera.getTransform().position.z = -2.0f;
}

void VisualizerScene::update(float dt)
{
    moveCamera(mainCamera, dt);
    rotateCamera(mainCamera, dt);
}

void VisualizerScene::draw()
{
    static MeshBuilder* meshBuilder = nullptr;

    if(!meshBuilder)
    {
        VertexFormat vtxFmt;

        vtxFmt.size = 1;
        vtxFmt[0].data = EMVF_ATTRB_USAGE_POS |
                         EMVF_ATTRB_TYPE_FLOAT |
                         EMVF_ATTRB_SIZE(3) |
                         EMVF_ATTRB_NORMALIZED_FALSE;

        meshBuilder = new MeshBuilder(vtxFmt);

        meshBuilder->index(6, 0, 1, 2, 0, 2, 3);
        meshBuilder->
        vertex(nullptr, -0.5f, -0.5f, 0.0f).
        vertex(nullptr,  0.5f, -0.5f, 0.0f).
        vertex(nullptr,  0.5f,  0.5f, 0.0f).
        vertex(nullptr, -0.5f,  0.5f, 0.0f);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    basicShader.setProjectionMatrix(mainCamera.getViewProjectionMatrix());
    basicShader.use();
    meshBuilder->drawElements(GL_TRIANGLES);
}

void VisualizerScene::destroy()
{
    basicShader.destroy();
}