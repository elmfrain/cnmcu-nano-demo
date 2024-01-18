#pragma once

#include <string>
#include <forward_list>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <shaders/Shader.hpp>
#include <LuaInclude.hpp>

#include "animation/Smoother.hpp"
#include "animation/Timeline.hpp"

#include "Logger.hpp"

namespace em
{
    struct Transform
    {
        enum RotationMode
        {
            EULER_XYZ = 0,
            EULER_XZY,
            EULER_YXZ,
            EULER_YZX,
            EULER_ZXY,
            EULER_ZYX,
            QUATERNION
        };

        static const char* rotationModeNames[];

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

        int lua_this(lua_State* L);
        static int lua_openTransformLib(lua_State* L);
    private:
        static int lua_getPosition(lua_State* L);
        static int lua_getRotationEuler(lua_State* L);
        static int lua_getRotationQuaternion(lua_State* L);
        static int lua_getScale(lua_State* L);
        static int lua_getOffset(lua_State* L);
        static int lua_getRotationMode(lua_State* L);

        static int lua_setPosition(lua_State* L);
        static int lua_setRotationEuler(lua_State* L);
        static int lua_setRotationQuaternion(lua_State* L);
        static int lua_setScale(lua_State* L);
        static int lua_setOffset(lua_State* L);
        static int lua_setRotationMode(lua_State* L);
    };

    class SceneObject;

    struct Dynamics : private LuaIndexable<Dynamics>
    {
        Dynamics();

        std::unordered_map<std::string, Smoother> smoothers;
        std::unordered_map<std::string, Timeline> timelines;
        bool enabled;

        void update(float dt, SceneObject* parent = nullptr);

        int lua_this(lua_State* L);
        static int lua_openDynamicsLib(lua_State* L);
    private:
        static lua_State* L;

        static int lua_getSmoother(lua_State* L);
        static int lua_getTimeline(lua_State* L);

        static int lua_deleteSmoother(lua_State* L);
        static int lua_deleteTimeline(lua_State* L);

        static int lua_isEnabled(lua_State* L);
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

        static const char* typeNames[];

        SceneObject(Type type, const std::string& name = "Object");
        virtual ~SceneObject();

        SceneObject(const SceneObject& other) = delete;
        SceneObject& operator=(const SceneObject& other) = delete;

        void doUpdate(float dt);
        virtual void draw(Shader& shader) = 0;

        Type getType() const;
        const std::string& getName() const;
        Transform& getTransform();
        const Transform& getConstTransform() const;
        void setName(const std::string& name);

        Dynamics& getDynamics();
        const Dynamics& getConstDynamics() const;

        void removeAllChildren();
        void removeChild(SceneObject& child);
        void addChild(SceneObject& child);
        int getChildCount() const;

        bool isDynamic() const;
        void setDynamic(bool dynamic);

        virtual int lua_this(lua_State* L);
        static int lua_openSceneObjectLib(lua_State* L);
    private:
        Type m_type;
        std::string m_name;

        Transform m_transform;
        Dynamics m_dynamics;

        SceneObject* m_parent;
        int m_numChildren;
        std::forward_list<SceneObject*> m_children;

        static int m_nextId;
        static lua_State* L;

        static int lua_getName(lua_State* L);
        static int lua_getType(lua_State* L);
        static int lua_getChildCount(lua_State* L);

        static int lua_removeAllChildren(lua_State* L);
        static int lua_removeChild(lua_State* L);
        static int lua_addChild(lua_State* L);

        static int lua_isDynamic(lua_State* L);
        static int lua_setDynamic(lua_State* L);

        static int lua_setName(lua_State* L);
    protected:
        int m_Id;

        virtual void update(float dt) = 0;

        static bool hasLuaInstance(int id);
        static void registerLuaInstance(int id);
        static std::unordered_map<std::string, SceneObject*>& getObjects();
        static const char* getRootName();
        static Logger logger;
    };
}