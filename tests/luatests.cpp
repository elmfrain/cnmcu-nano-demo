#include <gtest/gtest.h>

#include <VisualizerScene.hpp>
#include <MeshObject.hpp>
#include <cstring>

using namespace em;

static const char* luaError;
static bool luaErrorOccurred = false;
static char luaErrorBuffer[1024];

static int luaRun(lua_State* L, const char* str)
{
    int result = luaL_dostring(L, str);
    if (result != 0)
    {
        luaError = lua_tostring(L, -1);
        luaErrorOccurred = true;
        lua_pop(L, 1);
    }
    return result;
}

static int luaAssert(lua_State* L, const char* str)
{
    char buffer[1024];
    snprintf(buffer, 1024, "dummy = %s", str);
    int result = luaRun(L, buffer);
    if (result != 0) return result;

    snprintf(buffer, 1024, "assert(%s)", str);
    return luaRun(L, buffer);
}

static const char* luaGetError(const char* details)
{
    if(!luaErrorOccurred) return "";
    
    luaErrorOccurred = false;

    snprintf(luaErrorBuffer, 1024, "\n[Lua] %s:\n\t%s\n", details, luaError);
    return luaErrorBuffer;
}

//----------------------------------------------
// Tests
//----------------------------------------------

TEST(LuaScripting, Transform)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Transform::lua_openTransformLib(L);

    Transform t;
    t.position = glm::vec3(10.0f, 20.0f, 30.0f);
    t.rotationEuler = glm::vec3(40.0f, 50.0f, 60.0f);
    t.rotationQuaternion = glm::quat(40.0f, 50.0f, 60.0f, 70.0f);
    t.scale = glm::vec3(70.0f, 80.0f, 90.0f);
    t.offset = glm::vec3(100.0f, 110.0f, 120.0f);
    t.rotationMode = Transform::EULER_YZX;
    
    t.lua_this(L);
    lua_setglobal(L, "t");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(t).__name == 'Transform')"), 0) << luaGetError("Transform metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "t:getPosition()[1] == 10.0"), 0) << luaGetError("Transform::getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "t:getPosition()[2] == 20.0"), 0) << luaGetError("Transform::getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "t:getPosition()[3] == 30.0"), 0) << luaGetError("Transform::getPosition()[3] failed");

    ASSERT_EQ(luaAssert(L, "t:getRotationEuler()[1] == 40.0"), 0) << luaGetError("Transform::getRotationEuler()[1] failed");
    ASSERT_EQ(luaAssert(L, "t:getRotationEuler()[2] == 50.0"), 0) << luaGetError("Transform::getRotationEuler()[2] failed");
    ASSERT_EQ(luaAssert(L, "t:getRotationEuler()[3] == 60.0"), 0) << luaGetError("Transform::getRotationEuler()[3] failed");

    ASSERT_EQ(luaAssert(L, "t:getRotationQuaternion()[1] == 40.0"), 0) << luaGetError("Transform::getRotationQuaternion()[1] failed");
    ASSERT_EQ(luaAssert(L, "t:getRotationQuaternion()[2] == 50.0"), 0) << luaGetError("Transform::getRotationQuaternion()[2] failed");
    ASSERT_EQ(luaAssert(L, "t:getRotationQuaternion()[3] == 60.0"), 0) << luaGetError("Transform::getRotationQuaternion()[3] failed");
    ASSERT_EQ(luaAssert(L, "t:getRotationQuaternion()[4] == 70.0"), 0) << luaGetError("Transform::getRotationQuaternion()[4] failed");

    ASSERT_EQ(luaAssert(L, "t:getScale()[1] == 70.0"), 0) << luaGetError("Transform::getScale()[1] failed");
    ASSERT_EQ(luaAssert(L, "t:getScale()[2] == 80.0"), 0) << luaGetError("Transform::getScale()[2] failed"); 
    ASSERT_EQ(luaAssert(L, "t:getScale()[3] == 90.0"), 0) << luaGetError("Transform::getScale()[3] failed");

    ASSERT_EQ(luaAssert(L, "t:getOffset()[1] == 100.0"), 0) << luaGetError("Transform::getOffset()[1] failed");
    ASSERT_EQ(luaAssert(L, "t:getOffset()[2] == 110.0"), 0) << luaGetError("Transform::getOffset()[2] failed");
    ASSERT_EQ(luaAssert(L, "t:getOffset()[3] == 120.0"), 0) << luaGetError("Transform::getOffset()[3] failed");

    ASSERT_EQ(luaAssert(L, "t:getRotationMode() == \"EULER_YZX\""), 0) << luaGetError("Transform::getRotationMode() failed");

    ASSERT_EQ(luaRun(L, "t:setPosition({1.0, 2.0, 3.0})"), 0) << luaGetError("Transform::setPosition() failed");
    ASSERT_EQ(t.position, glm::vec3(1.0f, 2.0f, 3.0f)) << "Transform::setPosition() failed";

    ASSERT_EQ(luaRun(L, "t:setRotationEuler({4.0, 5.0, 6.0})"), 0) << luaGetError("Transform::setRotationEuler() failed");
    ASSERT_EQ(t.rotationEuler, glm::vec3(4.0f, 5.0f, 6.0f)) << "Transform::setRotationEuler() failed";

    ASSERT_EQ(luaRun(L, "t:setRotationQuaternion({7.0, 8.0, 9.0, 10.0})"), 0) << luaGetError("Transform::setRotationQuaternion() failed");
    ASSERT_EQ(t.rotationQuaternion, glm::quat(7.0f, 8.0f, 9.0f, 10.0f)) << "Transform::setRotationQuaternion() failed";

    ASSERT_EQ(luaRun(L, "t:setScale({11.0, 12.0, 13.0})"), 0) << luaGetError("Transform::setScale() failed");
    ASSERT_EQ(t.scale, glm::vec3(11.0f, 12.0f, 13.0f)) << "Transform::setScale() failed";

    ASSERT_EQ(luaRun(L, "t:setOffset({14.0, 15.0, 16.0})"), 0) << luaGetError("Transform::setOffset() failed");
    ASSERT_EQ(t.offset, glm::vec3(14.0f, 15.0f, 16.0f)) << "Transform::setOffset() failed";

    ASSERT_EQ(luaRun(L, "t:setRotationMode(\"EULER_XYZ\")"), 0) << luaGetError("Transform::setRotationMode() failed");
    ASSERT_EQ(t.rotationMode, Transform::EULER_XYZ) << "Transform::setRotationMode() failed";

    lua_close(L);
}

TEST(LuaScripting, PhongMaterial)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    PhongMaterial::lua_openPhongMaterialLib(L);

    PhongMaterial m;
    m.ambient = glm::vec3(10.0f, 20.0f, 30.0f);
    m.diffuse = glm::vec3(40.0f, 50.0f, 60.0f);
    m.specular = glm::vec3(70.0f, 80.0f, 90.0f);
    m.shininess = 100.0f;

    m.lua_this(L);
    lua_setglobal(L, "m");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(m).__name == 'PhongMaterial')"), 0) << luaGetError("PhongMaterial metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "m:getAmbient()[1] == 10.0"), 0) << luaGetError("PhongMaterial::getAmbient()[1] failed");
    ASSERT_EQ(luaAssert(L, "m:getAmbient()[2] == 20.0"), 0) << luaGetError("PhongMaterial::getAmbient()[2] failed");
    ASSERT_EQ(luaAssert(L, "m:getAmbient()[3] == 30.0"), 0) << luaGetError("PhongMaterial::getAmbient()[3] failed");

    ASSERT_EQ(luaAssert(L, "m:getDiffuse()[1] == 40.0"), 0) << luaGetError("PhongMaterial::getDiffuse()[1] failed");
    ASSERT_EQ(luaAssert(L, "m:getDiffuse()[2] == 50.0"), 0) << luaGetError("PhongMaterial::getDiffuse()[2] failed");
    ASSERT_EQ(luaAssert(L, "m:getDiffuse()[3] == 60.0"), 0) << luaGetError("PhongMaterial::getDiffuse()[3] failed");

    ASSERT_EQ(luaAssert(L, "m:getSpecular()[1] == 70.0"), 0) << luaGetError("PhongMaterial::getSpecular()[1] failed");
    ASSERT_EQ(luaAssert(L, "m:getSpecular()[2] == 80.0"), 0) << luaGetError("PhongMaterial::getSpecular()[2] failed");
    ASSERT_EQ(luaAssert(L, "m:getSpecular()[3] == 90.0"), 0) << luaGetError("PhongMaterial::getSpecular()[3] failed");

    ASSERT_EQ(luaAssert(L, "m:getShininess() == 100.0"), 0) << luaGetError("PhongMaterial::getShininess() failed");

    ASSERT_EQ(luaAssert(L, "math.abs(m:getRoughness() - 0.1) < 0.0001"), 0) << luaGetError("PhongMaterial::getRoughness() failed");

    ASSERT_EQ(luaRun(L, "m:setAmbient({1.0, 2.0, 3.0})"), 0) << luaGetError("PhongMaterial::setAmbient() failed");
    ASSERT_EQ(m.ambient, glm::vec3(1.0f, 2.0f, 3.0f)) << "PhongMaterial::setAmbient() failed";

    ASSERT_EQ(luaRun(L, "m:setDiffuse({4.0, 5.0, 6.0})"), 0) << luaGetError("PhongMaterial::setDiffuse() failed");
    ASSERT_EQ(m.diffuse, glm::vec3(4.0f, 5.0f, 6.0f)) << "PhongMaterial::setDiffuse() failed";

    ASSERT_EQ(luaRun(L, "m:setSpecular({7.0, 8.0, 9.0})"), 0) << luaGetError("PhongMaterial::setSpecular() failed");
    ASSERT_EQ(m.specular, glm::vec3(7.0f, 8.0f, 9.0f)) << "PhongMaterial::setSpecular() failed";

    ASSERT_EQ(luaRun(L, "m:setShininess(10.0)"), 0) << luaGetError("PhongMaterial::setShininess() failed");
    ASSERT_EQ(m.shininess, 10.0f) << "PhongMaterial::setShininess() failed";

    ASSERT_EQ(luaRun(L, "m:setRoughness(0.2)"), 0) << luaGetError("PhongMaterial::setRoughness() failed");
    ASSERT_FLOAT_EQ(m.shininess, 25.0f) << "PhongMaterial::setRoughness() failed";

    lua_close(L);
}

TEST(LuaScripting, Compositor)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Compositor::lua_openCompositorLib(L);

    Compositor c;
    c.setGamma(4.0f);
    c.setExposure(5.0f);

    c.lua_this(L);
    lua_setglobal(L, "c");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(c).__name == 'Compositor')"), 0) << luaGetError("Compositor metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "c:getGamma() == 4.0"), 0) << luaGetError("Compositor::getGamma() failed");

    ASSERT_EQ(luaAssert(L, "c:getExposure() == 5.0"), 0) << luaGetError("Compositor::getExposure() failed");

    ASSERT_EQ(luaRun(L, "c:setGamma(6.0)"), 0) << luaGetError("Compositor::setGamma() failed");
    ASSERT_EQ(c.getGamma(), 6.0f) << "Compositor::setGamma() failed";

    ASSERT_EQ(luaRun(L, "c:setExposure(7.0)"), 0) << luaGetError("Compositor::setExposure() failed");
    ASSERT_EQ(c.getExposure(), 7.0f) << "Compositor::setExposure() failed";

    lua_close(L);
}

TEST(LuaScripting, SceneObject)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    SceneObject::lua_openSceneObjectLib(L);

    class DummySceneObject : public SceneObject
    {
    public:
        DummySceneObject() : SceneObject(SceneObject::EMPTY) {}
        virtual void update(float dt) {}
        virtual void draw(Shader& shader) {}
    };

    DummySceneObject o;
    o.getTransform().position = glm::vec3(10.0f, 20.0f, 30.0f);
    o.setName("TestObject");
    DummySceneObject c1;
    c1.setName("TestChild1");
    DummySceneObject c2;
    c2.setName("TestChild2");
    DummySceneObject c3;
    c3.setName("TestChild3");

    o.addChild(c1);
    o.addChild(c2);

    o.lua_this(L);
    lua_setglobal(L, "o");
    c1.lua_this(L);
    lua_setglobal(L, "c1");
    c2.lua_this(L);
    lua_setglobal(L, "c2");
    c3.lua_this(L);
    lua_setglobal(L, "c3");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(o).__name == 'SceneObject')"), 0) << luaGetError("SceneObject metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "o:getType() == \"EMPTY\""), 0) << luaGetError("SceneObject::getType() failed");
    ASSERT_EQ(luaAssert(L, "o:getName() == \"TestObject\""), 0) << luaGetError("SceneObject::getName() failed");
    ASSERT_EQ(luaAssert(L, "o:getChildCount() == 2"), 0) << luaGetError("SceneObject::getChildCount() failed");

    ASSERT_EQ(luaAssert(L, "o.transform:getPosition()[1] == 10.0"), 0) << luaGetError("SceneObject::transform.getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "o.transform:getPosition()[2] == 20.0"), 0) << luaGetError("SceneObject::transform.getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "o.transform:getPosition()[3] == 30.0"), 0) << luaGetError("SceneObject::transform.getPosition()[3] failed");

    ASSERT_EQ(luaRun(L, "o:setName(\"TestObject2\")"), 0) << luaGetError("SceneObject::setName() failed");
    ASSERT_STREQ(o.getName().c_str(), "TestObject2") << "SceneObject::setName() failed";

    ASSERT_EQ(luaRun(L, "o:addChild(c3)"), 0) << luaGetError("SceneObject::addChild() failed");
    ASSERT_EQ(o.getChildCount(), 3) << "SceneObject::addChild() failed";

    ASSERT_EQ(luaRun(L, "o:removeChild(c2)"), 0) << luaGetError("SceneObject::removeChild() failed");
    ASSERT_EQ(o.getChildCount(), 2) << "SceneObject::removeChild() failed";

    ASSERT_EQ(luaRun(L, "o:removeAllChildren()"), 0) << luaGetError("SceneObject::removeAllChildren() failed");
    ASSERT_EQ(o.getChildCount(), 0) << "SceneObject::removeAllChildren() failed";

    lua_close(L);
}

TEST(LuaScripting, LightObject)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    LightObject::lua_openSceneObjectLib(L);

    LightObject l;
    l.getTransform().position = glm::vec3(10.0f, 20.0f, 30.0f);
    l.setColor(glm::vec3(40.0f, 50.0f, 60.0f));
    l.setIntensity(70.0f);

    l.lua_this(L);
    lua_setglobal(L, "l");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(l).__name == 'LightObject')"), 0) << luaGetError("LightObject metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "l:getType() == \"LIGHT\""), 0) << luaGetError("LightObject::getType() failed");

    ASSERT_EQ(luaAssert(L, "l.transform:getPosition()[1] == 10.0"), 0) << luaGetError("LightObject::transform.getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "l.transform:getPosition()[2] == 20.0"), 0) << luaGetError("LightObject::transform.getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "l.transform:getPosition()[3] == 30.0"), 0) << luaGetError("LightObject::transform.getPosition()[3] failed");

    ASSERT_EQ(luaAssert(L, "l:getColor()[1] == 40.0"), 0) << luaGetError("LightObject::getColor()[1] failed");
    ASSERT_EQ(luaAssert(L, "l:getColor()[2] == 50.0"), 0) << luaGetError("LightObject::getColor()[2] failed");
    ASSERT_EQ(luaAssert(L, "l:getColor()[3] == 60.0"), 0) << luaGetError("LightObject::getColor()[3] failed");

    ASSERT_EQ(luaAssert(L, "l:getIntensity() == 70.0"), 0) << luaGetError("LightObject::getIntensity() failed");

    ASSERT_EQ(luaRun(L, "l:setColor({1.0, 2.0, 3.0})"), 0) << luaGetError("LightObject::setColor() failed");
    ASSERT_EQ(l.getColor(), glm::vec3(1.0f, 2.0f, 3.0f)) << "LightObject::setColor() failed";

    ASSERT_EQ(luaRun(L, "l:setIntensity(4.0)"), 0) << luaGetError("LightObject::setIntensity() failed");
    ASSERT_EQ(l.getIntensity(), 4.0f) << "LightObject::setIntensity() failed";

    lua_close(L);
}

TEST(LuaScripting, MeshObject)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    MeshObject::lua_openSceneObjectLib(L);

    Mesh::Ptr mesh1 = Mesh::load("res/cube.obj")[0];
    Mesh::Ptr mesh2 = Mesh::load("res/cube.obj")[0];

    ASSERT_NE(mesh1, mesh2) << "Mesh::Ptr comparison operator failed";

    PhongMaterial material;
    material.ambient = glm::vec3(10.0f, 20.0f, 30.0f);

    MeshObject m;
    m.getTransform().position = glm::vec3(40.0f, 50.0f, 60.0f);
    m.setMesh(mesh1);
    m.setMaterial(material);

    m.lua_this(L);
    lua_setglobal(L, "m");
    luaPushSharedPtr<Mesh>(L, mesh2, "MeshPtr"); 
    lua_setglobal(L, "meshPtr2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(m).__name == 'MeshObject')"), 0) << luaGetError("MeshObject metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "m:getType() == \"MESH\""), 0) << luaGetError("MeshObject::getType() failed");

    ASSERT_EQ(luaAssert(L, "m.transform:getPosition()[1] == 40.0"), 0) << luaGetError("MeshObject::transform.getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "m.transform:getPosition()[2] == 50.0"), 0) << luaGetError("MeshObject::transform.getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "m.transform:getPosition()[3] == 60.0"), 0) << luaGetError("MeshObject::transform.getPosition()[3] failed");

    ASSERT_EQ(luaRun(L, "meshPtr1 = m:getMesh()"), 0) << luaGetError("MeshObject::getMesh() failed");
    lua_getglobal(L, "meshPtr1");
    Mesh::Ptr meshPtr1;
    luaGetSharedPtr<Mesh>(L, meshPtr1, "MeshPtr" -1);
    ASSERT_EQ(meshPtr1, mesh1) << "MeshObject::getMesh() failed";

    ASSERT_EQ(luaAssert(L, "m.material:getAmbient()[1] == 10.0"), 0) << luaGetError("MeshObject::material:getAmbient[1] failed");
    ASSERT_EQ(luaAssert(L, "m.material:getAmbient()[2] == 20.0"), 0) << luaGetError("MeshObject::material:getAmbient[2] failed");
    ASSERT_EQ(luaAssert(L, "m.material:getAmbient()[3] == 30.0"), 0) << luaGetError("MeshObject::material:getAmbient[3] failed");

    ASSERT_EQ(luaRun(L, "m:setMesh(meshPtr2)"), 0) << luaGetError("MeshObject::setMesh() failed");
    ASSERT_EQ(m.getMesh(), mesh2) << "MeshObject::setMesh() failed";

    lua_close(L); 
}