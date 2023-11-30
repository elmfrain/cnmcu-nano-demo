#include <Camera.hpp>
#include <Visualizer.hpp>

using namespace em;

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

void Camera::draw()
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