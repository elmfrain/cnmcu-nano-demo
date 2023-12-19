#include <SceneObject.hpp>
#include <cstring>

#include <LightObject.hpp>

using namespace em;

Logger SceneObject::logger("SceneObject");

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
    int error; \
    Transform* transform; \
    if((error = lua_getTransform(L, &transform)) != 0) \
        return error;

int Transform::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);

    lua_getglobal(L, "Transform");
    lua_setmetatable(L, -2);

    return 1;
}

int Transform::lua_openTransformLib(lua_State* L)
{
    static const luaL_Reg transformLib[] =
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

    luaL_newlib(L, transformLib);
    lua_setglobal(L, "Transform");

    lua_getglobal(L, "Transform");
    lua_pushstring(L, "__index");
    lua_getglobal(L, "Transform");
    lua_settable(L, -3);
    lua_pop(L, 1);

    return 0;
}

int Transform::lua_getTransform(lua_State* L, Transform** transform)
{
    luaPushValueFromKey("ptr", 1);
    luaGet(*transform, Transform*, userdata, -1);

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
    luaPushVec4(transform->rotationQuaternion);

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
    luaGetVec4(transform->rotationQuaternion, 2);

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
{
    setName(name);

    if(m_name != getRootName())
        getObjects()[getRootName()]->addChild(*this);
}

SceneObject::~SceneObject()
{
    if(m_name == getRootName())
    {
        logger.warnf("The root object is being deleted. This should not happen.");
        return;
    }

    removeAllChildren();

    if(m_parent)
        m_parent->removeChild(*this);

    getObjects().erase(m_name);
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
    if(m_name == getRootName())
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

// Removes all chidren and parents them to this object's parent
void SceneObject::removeAllChildren()
{
    if(m_children.empty() || m_name == getRootName())
        return;

    // Set all children's parent to this object's parent
    for(auto& child : m_children)
        child->m_parent = m_parent;

    // Add all children to this object's parent
    if(m_parent)
        m_parent->m_children.splice_after(m_parent->m_children.before_begin(), m_children);

    // Clear this object's children
    m_children.clear();
}

// Removes a child from this object's children and parents it to this object's
// parent
void SceneObject::removeChild(SceneObject& child)
{
    if(m_name == getRootName())
        return;

    // Set child's parent to this object's parent
    child.m_parent = m_parent;

    // Add child to this object's parent
    if(m_parent)
        m_parent->m_children.push_front(&child);

    // Remove child from this object's children
    m_children.remove(&child);
}

// Adds a child to this object's children and parents it to this object
void SceneObject::addChild(SceneObject& child)
{
    // Set child's parent to this object
    child.m_parent = this;

    // Add child to this object's children
    m_children.push_front(&child);
}

int SceneObject::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);
    lua_pushstring(L, "transform");
    m_transform.lua_this(L);
    lua_settable(L, -3);

    lua_getglobal(L, "SceneObject");
    lua_setmetatable(L, -2);

    return 1;
}

int SceneObject::lua_openSceneObjectLib(lua_State* L)
{
    static const luaL_Reg sceneObjectLib[] =
    {
        {"getName", lua_getName},
        {"setName", lua_setName},
        {"removeAllChildren", lua_removeAllChildren},
        {"removeChild", lua_removeChild},
        {"addChild", lua_addChild},
        {nullptr, nullptr}
    };

    Transform::lua_openTransformLib(L);

    luaL_newlib(L, sceneObjectLib);
    lua_setglobal(L, "SceneObject");

    lua_getglobal(L, "SceneObject");
    lua_pushstring(L, "__index");
    lua_getglobal(L, "SceneObject");
    lua_settable(L, -3);
    lua_pop(L, 1);

    LightObject::lua_openLightObjectLib(L);

    return 0;
}

#define luaGetSceneObject() \
    int error; \
    SceneObject* object; \
    if((error = lua_getSceneObject(L, &object)) != 0) \
        return error;

int SceneObject::lua_getSceneObject(lua_State* L, SceneObject** object, int index)
{
    luaPushValueFromKey("ptr", index);
    luaGet(*object, SceneObject*, userdata, -1);

    return 0;
}

int SceneObject::lua_getName(lua_State* L)
{
    luaGetSceneObject();
    lua_pushstring(L, object->getName().c_str());

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
    if(lua_getSceneObject(L, &child, 2) != 0)
        return luaL_error(L, "Expected a SceneObject as the first argument");

    object->removeChild(*child);

    return 0;
}

int SceneObject::lua_addChild(lua_State* L)
{
    luaGetSceneObject();
    SceneObject* child;
    if(lua_getSceneObject(L, &child, 2) != 0)
        return luaL_error(L, "Expected a SceneObject as the first argument");

    object->addChild(*child);

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
            : SceneObject(ROOT, getRootName())
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
        (*objects)[getRootName()] = new Root();
    }

    return *objects;
}

const char* SceneObject::getRootName()
{
    static const char* rootName = "Root";
    return rootName;
}