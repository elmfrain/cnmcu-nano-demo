#include <SceneObject.hpp>

namespace em 
{
    class Camera : public SceneObject
    {
    public:
        enum ProjectionMode
        {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };
        enum FOVMode
        {
            VERTICAL,
            HORIZONTAL
        };

        Camera(const std::string& name = "Camera");
        virtual ~Camera();

        virtual void update(float dt) override;
        virtual void draw() override;

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
    };
}