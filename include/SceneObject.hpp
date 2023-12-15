#pragma once

#include <string>
#include <forward_list>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <shaders/Shader.hpp>

#include "Logger.hpp"

namespace em
{
    struct Transform
    {
        enum RotationMode
        {
            EULER_XYZ,
            EULER_XZY,
            EULER_YXZ,
            EULER_YZX,
            EULER_ZXY,
            EULER_ZYX,
            QUATERNION
        };

        glm::vec3 position;
        glm::vec3 rotationEuler;
        glm::quat rotationQuaternion;
        glm::vec3 scale;

        glm::vec3 offset;
        RotationMode rotationMode;

        Transform(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));

        glm::mat4 getMatrix() const;
        glm::mat4 getInverseMatrix() const;
        glm::quat getRotationQuaternion() const;
    };

    class SceneObject
    {
    public:
        enum Type
        {
            EMPTY,
            MESH,
            LIGHT,
            CAMERA,
            ROOT
        };

        SceneObject(Type type, const std::string& name = "Object");
        virtual ~SceneObject();

        SceneObject(const SceneObject& other) = delete;
        SceneObject& operator=(const SceneObject& other) = delete;

        virtual void update(float dt) = 0;
        virtual void draw(Shader& shader) = 0;

        Type getType() const;
        const std::string& getName() const;
        Transform& getTransform();
        const Transform& getConstTransform() const;
        void setName(const std::string& name);

        void removeAllChildren();
        void removeChild(SceneObject& child);
        void addChild(SceneObject& child);
    private:
        Type m_type;
        std::string m_name;

        Transform m_transform;

        SceneObject* m_parent;
        std::forward_list<SceneObject*> m_children;
    protected:
        static std::unordered_map<std::string, SceneObject*>& getObjects();
        static const char* getRootName();
        static Logger logger;
    };
}