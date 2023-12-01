#include <SceneObject.hpp>

using namespace em;

Logger SceneObject::logger("SceneObject");

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

        void draw() override
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