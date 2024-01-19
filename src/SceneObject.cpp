#include <SceneObject.hpp>
#include <cstring>

#include <LightObject.hpp>
#include <MeshObject.hpp>
#include <Camera.hpp>

using namespace em;

Logger SceneObject::logger("SceneObject");

//----------------------------------------------------------------------------------------------
// Transform
//----------------------------------------------------------------------------------------------

const char* Transform::rotationModeNames[] =
{
    "EULER_XYZ",
    "EULER_XZY",
    "EULER_YXZ",
    "EULER_YZX",
    "EULER_ZXY",
    "EULER_ZYX",
    "QUATERNION"
};

Transform::Transform(const glm::vec3& position, const glm::vec3& scale)
    : position(position)
    , rotationEuler(0.0f)
    , rotationQuaternion(1.0f, 0.0f, 0.0f, 0.0f)
    , scale(scale)
    , offset(0.0f)
    , rotationMode(EULER_XYZ)
{
}

glm::mat4 Transform::getMatrix() const
{
    const glm::vec3 flipZVec = glm::vec3(1.0f, 1.0f, -1.0f);

    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position * flipZVec);
    matrix *= glm::mat4_cast(getRotationQuaternion());
    matrix = glm::translate(matrix, offset * flipZVec);
    matrix = glm::scale(matrix, scale);
    return matrix;
}

glm::mat4 Transform::getInverseMatrix() const
{
    const glm::vec3 flipZVec = glm::vec3(1.0f, 1.0f, -1.0f);

    glm::mat4 matrix = glm::scale(glm::mat4(1.0f), 1.0f / scale);
    matrix = glm::translate(matrix, -offset * flipZVec);
    matrix = glm::inverse(glm::mat4_cast(getRotationQuaternion())) * matrix;
    matrix = glm::translate(matrix, -position * flipZVec);
    return matrix;
}

glm::quat Transform::getRotationQuaternion() const
{
    switch(rotationMode)
    {
    case EULER_XYZ:
        return glm::quat(rotationEuler);
    case EULER_XZY:
        return glm::quat(glm::vec3(rotationEuler.x, rotationEuler.z, rotationEuler.y));
    case EULER_YXZ:
        return glm::quat(glm::vec3(rotationEuler.y, rotationEuler.x, rotationEuler.z));
    case EULER_YZX:
        return glm::quat(glm::vec3(rotationEuler.y, rotationEuler.z, rotationEuler.x));
    case EULER_ZXY:
        return glm::quat(glm::vec3(rotationEuler.z, rotationEuler.x, rotationEuler.y));
    case EULER_ZYX:
        return glm::quat(glm::vec3(rotationEuler.z, rotationEuler.y, rotationEuler.x));
    case QUATERNION:
        return rotationQuaternion;
    default:
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
}

#define luaGetTransform() \
    Transform* transform; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(transform, Transform, -1);

int Transform::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "Transform");
    lua_setmetatable(L, -2);

    return 1;
}

int Transform::lua_openTransformLib(lua_State* L)
{
    static const luaL_Reg luaTransformMethods[] =
    {
        {"getPosition", lua_getPosition},
        {"getRotationEuler", lua_getRotationEuler},
        {"getRotationQuaternion", lua_getRotationQuaternion},
        {"getScale", lua_getScale},
        {"getOffset", lua_getOffset},
        {"getRotationMode", lua_getRotationMode},
        {"setPosition", lua_setPosition},
        {"setRotationEuler", lua_setRotationEuler},
        {"setRotationQuaternion", lua_setRotationQuaternion},
        {"setScale", lua_setScale},
        {"setOffset", lua_setOffset},
        {"setRotationMode", lua_setRotationMode},
        {nullptr, nullptr}
    };

    luaL_newmetatable(L, "Transform");
    luaL_setfuncs(L, luaTransformMethods, 0);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_setglobal(L, "Transform");

    return 0;
}

int Transform::lua_getPosition(lua_State* L)
{
    luaGetTransform();
    luaPushVec3(transform->position);

    return 1;
}

int Transform::lua_getRotationEuler(lua_State* L)
{
    luaGetTransform();
    luaPushVec3(transform->rotationEuler);

    return 1;
}

int Transform::lua_getRotationQuaternion(lua_State* L)
{
    luaGetTransform();
    luaPushQuat(transform->rotationQuaternion);

    return 1;
}

int Transform::lua_getScale(lua_State* L)
{
    luaGetTransform();
    luaPushVec3(transform->scale);

    return 1;
}

int Transform::lua_getOffset(lua_State* L)
{
    luaGetTransform();
    luaPushVec3(transform->offset);

    return 1;
}

int Transform::lua_getRotationMode(lua_State* L)
{
    luaGetTransform();
    lua_pushstring(L, rotationModeNames[transform->rotationMode]);

    return 1;
}

int Transform::lua_setPosition(lua_State* L)
{
    luaGetTransform();
    luaGetVec3(transform->position, 2);

    return 0;
}

int Transform::lua_setRotationEuler(lua_State* L)
{
    luaGetTransform();
    luaGetVec3(transform->rotationEuler, 2);

    return 0;
}

int Transform::lua_setRotationQuaternion(lua_State* L)
{
    luaGetTransform();
    luaGetQuat(transform->rotationQuaternion, 2);

    return 0;
}

int Transform::lua_setScale(lua_State* L)
{
    luaGetTransform();
    luaGetVec3(transform->scale, 2);

    return 0;
}

int Transform::lua_setOffset(lua_State* L)
{
    luaGetTransform();
    luaGetVec3(transform->offset, 2);

    return 0;
}

int Transform::lua_setRotationMode(lua_State* L)
{
    luaGetTransform();
    const char* rotationModeName = luaL_checkstring(L, 2);

    for(int i = 0; i < 7; i++)
    {
        if(strcmp(rotationModeName, rotationModeNames[i]) == 0)
        {
            transform->rotationMode = (RotationMode)i;
            return 0;
        }
    }

    return luaL_error(L, "Invalid rotation mode %s", rotationModeName);
}

//----------------------------------------------------------------------------------------------
// Dynamics
//----------------------------------------------------------------------------------------------

lua_State* Dynamics::L = nullptr;

Dynamics::Dynamics() :
    enabled(false)
{
}

void Dynamics::update(float dt, SceneObject* parent)
{
    for(auto& smoother : smoothers)
        smoother.second.update(dt);

    for(auto& timeline: timelines)
        timeline.second.update(dt);

    if(!hasLuaInstance(L))
        return;

    lua_getfield(L, -1, "onUpdate");

    if(lua_isfunction(L, -1))
    {
        lua_pushnumber(L, dt);

        if(parent)
        {
            parent->lua_this(L);
            lua_call(L, 2, 0);
        }
        else lua_call(L, 1, 0);
    } else lua_pop(L, 1);

    lua_pop(L, 1);
}

#define luaGetDynamics() \
    Dynamics* dynamics; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(dynamics, Dynamics, -1);

int Dynamics::lua_this(lua_State* L)
{
    if(hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "Dynamics");
    lua_setmetatable(L, -2);

    luaRegisterInstance(L);

    return 1;
}

int Dynamics::lua_openDynamicsLib(lua_State* L)
{
    static const luaL_Reg luaDynamicsMethods[] =
    {
        {"getSmoother", lua_getSmoother},
        {"getTimeline", lua_getTimeline},
        {"deleteTimeline", lua_deleteTimeline},
        {"deleteSmoother", lua_deleteSmoother},
        {"isEnabled", lua_isEnabled},
        {nullptr, nullptr}
    };

    Dynamics::L = L;
    Smoother::lua_openSmootherLib(L);
    Timeline::lua_openTimelineLib(L);

    luaL_newmetatable(L, "Dynamics");
    luaL_setfuncs(L, luaDynamicsMethods, 0);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_setglobal(L, "Dynamics");

    LuaIndexable<Dynamics>::luaRegisterType(L);

    return 0;
}

int Dynamics::lua_getSmoother(lua_State* L)
{
    luaGetDynamics();
    
    const char* smootherName = luaL_checkstring(L, 2);
    dynamics->smoothers[smootherName].lua_this(L);

    return 1;
}

int Dynamics::lua_getTimeline(lua_State* L)
{
    luaGetDynamics();

    const char* timelineName = luaL_checkstring(L, 2);
    dynamics->timelines[timelineName].lua_this(L);

    return 1;
}

int Dynamics::lua_deleteSmoother(lua_State* L)
{
    luaGetDynamics();

    const char* smootherName = luaL_checkstring(L, 2);
    dynamics->smoothers.erase(smootherName);

    return 0;
}

int Dynamics::lua_deleteTimeline(lua_State* L)
{
    luaGetDynamics();

    const char* timelineName = luaL_checkstring(L, 2);
    dynamics->timelines.erase(timelineName);

    return 0;
}

int Dynamics::lua_isEnabled(lua_State* L)
{
    luaGetDynamics();
    lua_pushboolean(L, dynamics->enabled);

    return 1;
}

//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// SceneObject
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------

const char* SceneObject::typeNames[] =
{
    "EMPTY",
    "MESH",
    "LIGHT",
    "CAMERA",
    "ROOT"
};

SceneObject::SceneObject(Type type, const std::string& name)
    : m_type(type)
    , m_parent(nullptr)
    , m_numChildren(0)
{
    setName(name);

    if(m_name != rootObjName())
        getObjects()[rootObjName()]->addChild(*this);
}

SceneObject::~SceneObject()
{
    if(m_name == rootObjName())
    {
        logger.warnf("The root object is being deleted. This should not happen.");
        return;
    }

    removeAllChildren();

    if(m_parent)
        m_parent->removeChild(*this);

    getObjects().erase(m_name);
}

void SceneObject::doUpdate(float dt)
{
    if(m_name == rootObjName())
        return;

    if(m_dynamics.enabled)
        m_dynamics.update(dt, this);

    update(dt);
}

SceneObject::Type SceneObject::getType() const
{
    return m_type;
}

const std::string& SceneObject::getName() const
{
    return m_name;
}

Transform& SceneObject::getTransform()
{
    return m_transform;
}

const Transform& SceneObject::getConstTransform() const
{
    return m_transform;
}

// Sets the name of the object. If the name is already taken, it will be
// appended with a number.
void SceneObject::setName(const std::string& name)
{
    if(m_name == rootObjName())
        return;

    getObjects().erase(m_name);

    int i = 0;
    std::string newName = name;
    char extension[32] = {0};

    while(getObjects().count(newName) > 0)
    {
        sprintf(extension, ".%03d", i++);
        newName = name + extension;
    }

    m_name = newName;

    getObjects()[newName] = this;
}

Dynamics& SceneObject::getDynamics()
{
    return m_dynamics;
}

const Dynamics& SceneObject::getConstDynamics() const
{
    return m_dynamics;
}

// Removes all chidren and parents them to this object's parent
void SceneObject::removeAllChildren()
{
    if(m_children.empty() || m_name == rootObjName())
        return;

    // Set all children's parent to this object's parent
    for(auto& child : m_children)
        child->m_parent = m_parent;

    // Add all children to this object's parent
    if(m_parent)
    {
        m_parent->m_children.splice_after(m_parent->m_children.before_begin(), m_children);
        m_parent->m_numChildren += m_numChildren;
    }

    // Clear this object's children
    m_children.clear();
    m_numChildren = 0;
}

// Removes a child from this object's children and parents it to this object's
// parent
void SceneObject::removeChild(SceneObject& child)
{
    if(m_name == rootObjName())
        return;

    // Set child's parent to this object's parent
    child.m_parent = m_parent;

    // Add child to this object's parent
    if(m_parent)
    {
        m_parent->m_children.push_front(&child);
        m_parent->m_numChildren++;
    }

    // Remove child from this object's children
    m_children.remove(&child);
    m_numChildren--;
}

// Adds a child to this object's children and parents it to this object
void SceneObject::addChild(SceneObject& child)
{
    // Set child's parent to this object
    child.m_parent = this;

    // Add child to this object's children
    m_children.push_front(&child);
    m_numChildren++;
}

int SceneObject::getChildCount() const
{
    return m_numChildren;
}

bool SceneObject::isDynamic() const
{
    return m_dynamics.enabled;
}

void SceneObject::setDynamic(bool dynamic)
{
    m_dynamics.enabled = dynamic;
}

int SceneObject::lua_this(lua_State* L)
{
    if(hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);
    lua_pushstring(L, "transform");
    m_transform.lua_this(L);
    lua_settable(L, -3);
    lua_pushstring(L, "dynamics");
    m_dynamics.lua_this(L);
    lua_settable(L, -3);

    luaL_newmetatable(L, "SceneObject");
    lua_setmetatable(L, -2);

    luaRegisterInstance(L);

    return 1;
}

int SceneObject::lua_openSceneObjectLib(lua_State* L)
{
    static const luaL_Reg luaSceneObjectMethods[] =
    {
        {"getName", lua_getName},
        {"getType", lua_getType},
        {"getChildCount", lua_getChildCount},
        {"setName", lua_setName},
        {"removeAllChildren", lua_removeAllChildren},
        {"removeChild", lua_removeChild},
        {"addChild", lua_addChild},
        {"isDynamic", lua_isDynamic},
        {"setDynamic", lua_setDynamic},
        {nullptr, nullptr}
    };
    Transform::lua_openTransformLib(L);
    Dynamics::lua_openDynamicsLib(L);

    luaL_newmetatable(L, "SceneObject");
    luaL_setfuncs(L, luaSceneObjectMethods, 0);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_setglobal(L, "SceneObject");

    LuaIndexable<SceneObject>::luaRegisterType(L);

    LightObject::lua_openLightObjectLib(L);
    MeshObject::lua_openMeshObjectLib(L);
    Camera::lua_openCameraLib(L);

    return 0;
}

#define luaGetSceneObject() \
    SceneObject* object; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(object, SceneObject, -1);

int SceneObject::lua_getName(lua_State* L)
{
    luaGetSceneObject();
    lua_pushstring(L, object->getName().c_str());

    return 1;
}

int SceneObject::lua_getType(lua_State* L)
{
    luaGetSceneObject();
    lua_pushstring(L, typeNames[object->getType()]);

    return 1;
}

int SceneObject::lua_getChildCount(lua_State* L)
{
    luaGetSceneObject();
    lua_pushinteger(L, object->getChildCount());

    return 1;
}

int SceneObject::lua_removeAllChildren(lua_State* L)
{
    luaGetSceneObject();
    object->removeAllChildren();

    return 0;
}

int SceneObject::lua_removeChild(lua_State* L)
{
    luaGetSceneObject();
    SceneObject* child;
    luaPushValueFromKey("ptr", 2);
    luaGetPointer(child, SceneObject, -1);

    object->removeChild(*child);

    return 0;
}

int SceneObject::lua_addChild(lua_State* L)
{
    luaGetSceneObject();
    SceneObject* child;
    luaPushValueFromKey("ptr", 2);
    luaGetPointer(child, SceneObject, -1);

    object->addChild(*child);

    return 0;
}

int SceneObject::lua_isDynamic(lua_State* L)
{
    luaGetSceneObject();
    lua_pushboolean(L, object->isDynamic());

    return 1;
}

int SceneObject::lua_setDynamic(lua_State* L)
{
    luaGetSceneObject();
    bool dynamic;
    luaGet(dynamic, bool, boolean, 2);

    object->setDynamic(dynamic);

    return 0;
}

int SceneObject::lua_setName(lua_State* L)
{
    luaGetSceneObject();
    const char* name = luaL_checkstring(L, 2);

    object->setName(name);

    return 0;
}

std::unordered_map<std::string, SceneObject*>& SceneObject::getObjects()
{
    static std::unordered_map<std::string, SceneObject*>* objects = nullptr;
    
    class Root : public SceneObject
    {
    public:
        Root()
            : SceneObject(ROOT, rootObjName())
        {
        }

        ~Root()
        {
        }

        void update(float dt) override
        {
        }

        void draw(Shader& shader) override
        {
        }
    };

    if(!objects)
    {
        objects = new std::unordered_map<std::string, SceneObject*>();
        (*objects)[rootObjName()] = new Root();
    }

    return *objects;
}

const char* SceneObject::rootObjName()
{
    static const char* rootName = "Root";
    return rootName;
}