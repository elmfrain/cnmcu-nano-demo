#include <gtest/gtest.h>

#include <VisualizerScene.hpp>
#include <MeshObject.hpp>
#include <cstring>
#include <animation/Timeline.hpp>

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

class DummyIndexable : public LuaIndexable<DummyIndexable>
{
public:
    int m_value;

    DummyIndexable() : m_value(0) {}
    
    int lua_this(lua_State* L)
    {
        if(hasLuaInstance(L))
            return 1;
        lua_newtable(L);
        lua_pushlightuserdata(L, this);
        lua_setfield(L, -2, "ptr");
        luaL_newmetatable(L, "DummyIndexable");
        lua_setmetatable(L, -2);
        luaRegisterInstance(L);
        return 1;
    }
    static int lua_getValue(lua_State* L)
    {
        DummyIndexable* dummyIndexable = nullptr;
        luaPushValueFromKey("ptr", 1);
        luaGetPointer(dummyIndexable, DummyIndexable, -1);
        lua_pushinteger(L, dummyIndexable->m_value);
        return 1;
    }
    static int lua_setValue(lua_State* L)
    {
        DummyIndexable* dummyIndexable = nullptr;
        luaPushValueFromKey("ptr", 1);
        luaGetPointer(dummyIndexable, DummyIndexable, -1);
        dummyIndexable->m_value = lua_tointeger(L, 2);
        return 0;
    }
    static int lua_openDummyIndexableLib(lua_State* L)
    {
        luaL_Reg dummyIndexableMethods[] =
        {
            { "getValue", lua_getValue },
            { "setValue", lua_setValue },
            { NULL, NULL }
        };
        luaL_newmetatable(L, "DummyIndexable");
        luaL_setfuncs(L, dummyIndexableMethods, 0);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        lua_setglobal(L, "DummyIndexable");
        LuaIndexable<DummyIndexable>::luaRegisterType(L);
        return 1;
    }
};

template<>
const char* LuaIndexable<DummyIndexable>::luaIndexTableName()
{
    return "__DummyIndexableInstances";
}

TEST(LuaScripting, LuaInclude)
{
    static lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    //--------------------------------------------------------------------------------
    // Test luaGet macro
    //--------------------------------------------------------------------------------

    lua_pushstring(L, "test");
    lua_setglobal(L, "testStr");
    lua_pushnumber(L, 10.0);
    lua_setglobal(L, "testNum");
    lua_pushboolean(L, true);
    lua_setglobal(L, "testBool");
    lua_pushlightuserdata(L, (void*)0xdeadbeef); // Lol dead beef, who came up with that?
    lua_setglobal(L, "testPtr");

    auto getStr = [](lua_State*& L, const char*& str){ luaGet(str, const char*, string, -1); return 0; };
    auto getNum = [](lua_State*& L, double& num){ luaGet(num, double, number, -1); return 0; };
    auto getBool = [](lua_State*& L, bool& b){ luaGet(b, bool, boolean, -1); return 0; };
    auto getPtr = [](lua_State*& L, void*& ptr){ luaGet(ptr, void*, userdata, -1); return 0; };

    const char* str;
    lua_getglobal(L, "testStr");
    getStr(L, str);
    ASSERT_STREQ(str, "test") << "luaGet() failed";

    double num;
    lua_getglobal(L, "testNum");
    getNum(L, num);
    ASSERT_EQ(num, 10.0) << "luaGet() failed";

    bool b;
    lua_getglobal(L, "testBool");
    getBool(L, b);
    ASSERT_EQ(b, true) << "luaGet() failed";

    void* ptr;
    lua_getglobal(L, "testPtr");
    getPtr(L, ptr);
    ASSERT_EQ(ptr, (void*)0xdeadbeef) << "luaGet() failed";

    auto getPointer = [](lua_State*& L, int*& ptr){ luaGetPointer(ptr, int, -1) return 0; };

    int* ptr2;
    lua_getglobal(L, "testPtr");
    getPointer(L, ptr2);
    ASSERT_EQ(ptr2, (int*)0xdeadbeef) << "luaGetPointer() failed";

    //--------------------------------------------------------------------------------
    // Test vec2, vec3, vec4, quat push and get macros
    //--------------------------------------------------------------------------------

    glm::vec2 v2 = glm::vec2(10.0f, 20.0f);
    glm::vec3 v3 = glm::vec3(30.0f, 40.0f, 50.0f);
    glm::vec4 v4 = glm::vec4(60.0f, 70.0f, 80.0f, 90.0f);
    glm::quat q = glm::quat(100.0f, 110.0f, 120.0f, 130.0f);

    luaPushVec2(v2);
    lua_setglobal(L, "v2");
    luaPushVec3(v3);
    lua_setglobal(L, "v3");
    luaPushVec4(v4);
    lua_setglobal(L, "v4");
    luaPushQuat(q);
    lua_setglobal(L, "q");

    ASSERT_EQ(luaAssert(L, "v2[1] == 10.0"), 0) << luaGetError("luaPushVec2() failed");
    ASSERT_EQ(luaAssert(L, "v2[2] == 20.0"), 0) << luaGetError("luaPushVec2() failed");

    ASSERT_EQ(luaAssert(L, "v3[1] == 30.0"), 0) << luaGetError("luaPushVec3() failed");
    ASSERT_EQ(luaAssert(L, "v3[2] == 40.0"), 0) << luaGetError("luaPushVec3() failed");
    ASSERT_EQ(luaAssert(L, "v3[3] == 50.0"), 0) << luaGetError("luaPushVec3() failed");

    ASSERT_EQ(luaAssert(L, "v4[1] == 60.0"), 0) << luaGetError("luaPushVec4() failed");
    ASSERT_EQ(luaAssert(L, "v4[2] == 70.0"), 0) << luaGetError("luaPushVec4() failed");
    ASSERT_EQ(luaAssert(L, "v4[3] == 80.0"), 0) << luaGetError("luaPushVec4() failed");
    ASSERT_EQ(luaAssert(L, "v4[4] == 90.0"), 0) << luaGetError("luaPushVec4() failed");

    ASSERT_EQ(luaAssert(L, "q[1] == 100.0"), 0) << luaGetError("luaPushQuat() failed");
    ASSERT_EQ(luaAssert(L, "q[2] == 110.0"), 0) << luaGetError("luaPushQuat() failed");
    ASSERT_EQ(luaAssert(L, "q[3] == 120.0"), 0) << luaGetError("luaPushQuat() failed");
    ASSERT_EQ(luaAssert(L, "q[4] == 130.0"), 0) << luaGetError("luaPushQuat() failed");

    ASSERT_EQ(luaRun(L, "v2 = {1.0, 2.0}"), 0) << luaGetError("luaPushVec2() failed");
    lua_getglobal(L, "v2");
    auto getVec2 = [](lua_State*& L, glm::vec2& v2){ luaGetVec2(v2, -1); return 0; };
    getVec2(L, v2);
    ASSERT_EQ(v2, glm::vec2(1.0f, 2.0f)) << "luaPushVec2() failed";

    ASSERT_EQ(luaRun(L, "v3 = {3.0, 4.0, 5.0}"), 0) << luaGetError("luaPushVec3() failed");
    lua_getglobal(L, "v3");
    auto getVec3 = [](lua_State*& L, glm::vec3& v3){ luaGetVec3(v3, -1); return 0; };
    getVec3(L, v3);
    ASSERT_EQ(v3, glm::vec3(3.0f, 4.0f, 5.0f)) << "luaPushVec3() failed";

    ASSERT_EQ(luaRun(L, "v4 = {6.0, 7.0, 8.0, 9.0}"), 0) << luaGetError("luaPushVec4() failed");
    lua_getglobal(L, "v4");
    auto getVec4 = [](lua_State*& L, glm::vec4& v4){ luaGetVec4(v4, -1); return 0; };
    getVec4(L, v4);
    ASSERT_EQ(v4, glm::vec4(6.0f, 7.0f, 8.0f, 9.0f)) << "luaPushVec4() failed";

    //--------------------------------------------------------------------------------
    // Test luaPushValueFromKey
    //--------------------------------------------------------------------------------

    ASSERT_EQ(luaRun(L, "t = {webo = 11}"), 0) << luaGetError("luaPushValueFromKey() failed");
    auto getValue = [](lua_State*& L, int& value){ luaPushValueFromKey("webo", -1); value = lua_tonumber(L, -1); lua_pop(L, 1); return 0; };
    int value;
    lua_getglobal(L, "t");
    getValue(L, value);
    ASSERT_EQ(value, 11) << "luaPushValueFromKey() failed";

    //--------------------------------------------------------------------------------
    // Test LuaIndexable
    //--------------------------------------------------------------------------------

    DummyIndexable::lua_openDummyIndexableLib(L);

    lua_getglobal(L, "__DummyIndexableInstances");
    ASSERT_FALSE(lua_isnil(L, -1)) << "LuaIndexable::indexTable failed";
    lua_pop(L, 1);

    {
    DummyIndexable dummyIndexable;

    dummyIndexable.lua_this(L);
    lua_setglobal(L, "dummyIndexable");
    dummyIndexable.lua_this(L);
    lua_setglobal(L, "dummyIndexable2");

    lua_getglobal(L, "__DummyIndexableInstances");
    lua_pushinteger(L, 0);
    lua_gettable(L, -2);
    ASSERT_FALSE(lua_isnil(L, -1)) << "LuaIndexable::indexTable failed";
    lua_pop(L, 2);

    ASSERT_EQ(luaRun(L, "assert(getmetatable(dummyIndexable).__name == 'DummyIndexable')"), 0) << luaGetError("DummyIndexable metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "dummyIndexable == dummyIndexable2"), 0) << luaGetError("DummyIndexable::operator==() failed");

    ASSERT_EQ(luaRun(L, "dummyIndexable:setValue(10)"), 0) << luaGetError("DummyIndexable::setValue() failed");
    ASSERT_EQ(dummyIndexable.m_value, 10) << "DummyIndexable::setValue() failed";

    ASSERT_EQ(luaAssert(L, "dummyIndexable:getValue() == 10"), 0) << luaGetError("DummyIndexable::getValue() failed");
    ASSERT_EQ(luaAssert(L, "dummyIndexable2:getValue() == 10"), 0) << luaGetError("DummyIndexable::getValue() failed");
    }

    lua_getglobal(L, "__DummyIndexableInstances");
    lua_pushinteger(L, 0);
    lua_gettable(L, -2);
    ASSERT_TRUE(lua_isnil(L, -1)) << "LuaIndexable::hasLuaInstance() failed";

    lua_close(L);
}

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

TEST(LuaScripting, Smoother)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Smoother::lua_openSmootherLib(L);

    { // Runtime scope

    Smoother s;
    s.setSpeed(10.0f);
    s.setDamping(20.0f);
    s.setValue(30.0f);
    s.grab(40.0f);
    s.setSpringing(true);

    s.lua_this(L);
    lua_setglobal(L, "s");
    s.lua_this(L);
    lua_setglobal(L, "s2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(s).__name == 'Smoother')"), 0) << luaGetError("Smoother metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "s == s2"), 0) << luaGetError("Smoother::operator==() failed");

    ASSERT_EQ(luaAssert(L, "s:getValue() == 30.0"), 0) << luaGetError("Smoother::getValue() failed");
    ASSERT_EQ(luaAssert(L, "s:getTarget() == 40.0"), 0) << luaGetError("Smoother::getTarget() failed");
    ASSERT_EQ(luaAssert(L, "s:getSpeed() == 10.0"), 0) << luaGetError("Smoother::getSpeed() failed");
    ASSERT_EQ(luaAssert(L, "s:getDamping() == 20.0"), 0) << luaGetError("Smoother::getDamping() failed");

    ASSERT_EQ(luaRun(L, "s:setValue(40.0)"), 0) << luaGetError("Smoother::setValue() failed");
    ASSERT_EQ(s.getValue(), 40.0f) << "Smoother::setValue() failed";

    ASSERT_EQ(luaRun(L, "s:setSpringing(false)"), 0) << luaGetError("Smoother::setSpringing() failed");
    ASSERT_EQ(s.isSpringing(), false) << "Smoother::setSpringing() failed";

    ASSERT_EQ(luaRun(L, "s:setSpeed(50.0)"), 0) << luaGetError("Smoother::setSpeed() failed");
    ASSERT_EQ(s.getSpeed(), 50.0f) << "Smoother::setSpeed() failed";

    ASSERT_EQ(luaRun(L, "s:setDamping(60.0)"), 0) << luaGetError("Smoother::setDamping() failed");
    ASSERT_EQ(s.getDamping(), 60.0f) << "Smoother::setDamping() failed";

    ASSERT_EQ(luaRun(L, "s:release()"), 0) << luaGetError("Smoother::release() failed");
    ASSERT_EQ(s.isGrabbed(), false) << "Smoother::release() failed";

    ASSERT_EQ(luaRun(L, "s:grab()"), 0) << luaGetError("Smoother::grab() failed");
    ASSERT_EQ(s.isGrabbed(), true) << "Smoother::grab() failed";

    ASSERT_EQ(luaRun(L, "s:grab(70.0)"), 0) << luaGetError("Smoother::grab() failed");
    ASSERT_EQ(s.getTarget(), 70.0f) << "Smoother::grab() failed";

    ASSERT_EQ(luaRun(L, "s:setValueAndGrab(80.0)"), 0) << luaGetError("Smoother::setValueAndGrab() failed");
    ASSERT_EQ(s.getValue(), 80.0f) << "Smoother::setValueAndGrab() failed";
    ASSERT_EQ(s.getTarget(), 80.0f) << "Smoother::setValueAndGrab() failed";

    }

    lua_close(L);
}

TEST(LuaScripting, Keyframe)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Keyframe::lua_openKeyframeLib(L);

    { // Runtime scope

    Keyframe k;
    k.time = 10.0f;
    k.value = 20.0f;
    k.easing = Easing::quadIn;

    k.lua_this(L);
    lua_setglobal(L, "k");
    k.lua_this(L);
    lua_setglobal(L, "k2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(k).__name == 'Keyframe')"), 0) << luaGetError("Keyframe metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "k == k2"), 0) << luaGetError("Keyframe::operator==() failed");

    ASSERT_EQ(luaAssert(L, "k:getTime() == 10.0"), 0) << luaGetError("Keyframe::getTime() failed");
    ASSERT_EQ(luaAssert(L, "k:getValue() == 20.0"), 0) << luaGetError("Keyframe::getValue() failed");
    ASSERT_EQ(luaAssert(L, "k:getEasing() == \"QUAD_IN\""), 0) << luaGetError("Keyframe::getEasing() failed");

    ASSERT_EQ(luaRun(L, "k:setTime(30.0)"), 0) << luaGetError("Keyframe::setTime() failed");
    ASSERT_EQ(k.time, 30.0f) << "Keyframe::setTime() failed";

    ASSERT_EQ(luaRun(L, "k:setValue(40.0)"), 0) << luaGetError("Keyframe::setValue() failed");
    ASSERT_EQ(k.value, 40.0f) << "Keyframe::setValue() failed";

    ASSERT_EQ(luaRun(L, "k:setEasing(\"QUAD_OUT\")"), 0) << luaGetError("Keyframe::setEasing() failed");
    ASSERT_EQ(k.easing, Easing::quadOut) << "Keyframe::setEasing() failed";

    }

    lua_close(L);
}

TEST(LuaScripting, Track)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Track::lua_openTrackLib(L);

    { // Runtime scope

    Track t("test", 10.0f, 20.0f, Easing::quadInOut);

    t.lua_this(L);
    lua_setglobal(L, "t");
    t.lua_this(L);
    lua_setglobal(L, "t2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(t).__name == 'Track')"), 0) << luaGetError("Track metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "t == t2"), 0) << luaGetError("Track::operator==() failed");

    ASSERT_EQ(luaAssert(L, "t:getName() == \"test\""), 0) << luaGetError("Track::getName() failed");
    ASSERT_EQ(luaAssert(L, "t:getKeyframesCount() == 2"), 0) << luaGetError("Track::getKeyframesCount() failed");

    ASSERT_EQ(luaAssert(L, "t:getValue(0.0) == 10.0"), 0) << luaGetError("Track::getValue() failed");
    ASSERT_EQ(luaAssert(L, "t:getValue(0.5) == 15.0"), 0) << luaGetError("Track::getValue() failed");
    ASSERT_EQ(luaAssert(L, "t:getValue(1.0) == 20.0"), 0) << luaGetError("Track::getValue() failed");

    ASSERT_EQ(luaRun(L, "t:addKeyframe(0.5, 15.0, \"QUAD_IN\")"), 0) << luaGetError("Track::addKeyframe() failed");
    ASSERT_EQ(t.getKeyframesCount(), 3) << "Track::addKeyframe() failed";

    }

    lua_close(L);
}

TEST(LuaScripting, Timeline)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Timeline::lua_openTimelineLib(L);

    { // Runtime scope

    Timeline t;
    t.addTrack("test", 10.0f, 20.0f);
    t.setDuration(30.0f);
    t.setCurrentTime(15.0f);
    t.setSpeed(40.0f);
    t.setLooping(true);

    t.lua_this(L);
    lua_setglobal(L, "t");
    t.lua_this(L);
    lua_setglobal(L, "t2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(t).__name == 'Timeline')"), 0) << luaGetError("Timeline metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "t == t2"), 0) << luaGetError("Timeline::operator==() failed");

    ASSERT_EQ(luaAssert(L, "t:getValue(\"test\") == 15.0"), 0) << luaGetError("Timeline::getValue() failed");
    ASSERT_EQ(luaAssert(L, "t:getValue(0) == 15.0"), 0) << luaGetError("Timeline::getValue() failed");
    ASSERT_EQ(luaAssert(L, "t:getName(0) == \"test\""), 0) << luaGetError("Timeline::getName() failed");
    ASSERT_EQ(luaAssert(L, "t:getTracksCount() == 1"), 0) << luaGetError("Timeline::getTracksCount() failed");
    ASSERT_EQ(luaAssert(L, "t:getTrack(0):getName() == \"test\""), 0) << luaGetError("Timeline::getTrack() failed");
    ASSERT_EQ(luaAssert(L, "t:getTrack(\"test\"):getName() == \"test\""), 0) << luaGetError("Timeline::getTrack() failed");
    ASSERT_EQ(luaAssert(L, "t:getDuration() == 30.0"), 0) << luaGetError("Timeline::getDuration() failed");
    ASSERT_EQ(luaAssert(L, "t:getCurrentTime() == 15.0"), 0) << luaGetError("Timeline::getCurrentTime() failed");
    ASSERT_EQ(luaAssert(L, "t:getSpeed() == 40.0"), 0) << luaGetError("Timeline::getSpeed() failed");
    ASSERT_EQ(luaAssert(L, "t:isPaused() == false"), 0) << luaGetError("Timeline::isPaused() failed");
    ASSERT_EQ(luaAssert(L, "t:isStopped() == true"), 0) << luaGetError("Timeline::isStopped() failed");
    ASSERT_EQ(luaAssert(L, "t:isRewinding() == false"), 0) << luaGetError("Timeline::isRewinding() failed");
    ASSERT_EQ(luaAssert(L, "t:isLooping() == true"), 0) << luaGetError("Timeline::isLooping() failed");

    ASSERT_EQ(luaRun(L, "t:setDuration(50.0)"), 0) << luaGetError("Timeline::setDuration() failed");
    ASSERT_EQ(t.getDuration(), 50.0f) << "Timeline::setDuration() failed";

    ASSERT_EQ(luaRun(L, "t:setCurrentTime(25.0)"), 0) << luaGetError("Timeline::setCurrentTime() failed");
    ASSERT_EQ(t.getCurrentTime(), 25.0f) << "Timeline::setCurrentTime() failed";

    ASSERT_EQ(luaRun(L, "t:setSpeed(60.0)"), 0) << luaGetError("Timeline::setSpeed() failed");
    ASSERT_EQ(t.getSpeed(), 60.0f) << "Timeline::setSpeed() failed";

    ASSERT_EQ(luaRun(L, "t:setLooping(false)"), 0) << luaGetError("Timeline::setLooping() failed");
    ASSERT_FALSE(t.isLooping()) << "Timeline::setLooping() failed";

    ASSERT_EQ(luaRun(L, "t:play()"), 0) << luaGetError("Timeline::play() failed");
    ASSERT_FALSE(t.isPaused()) << "Timeline::play() failed";

    ASSERT_EQ(luaRun(L, "t:pause()"), 0) << luaGetError("Timeline::pause() failed");
    ASSERT_TRUE(t.isPaused()) << "Timeline::pause() failed";

    ASSERT_EQ(luaRun(L, "t:stop()"), 0) << luaGetError("Timeline::stop() failed");
    ASSERT_TRUE(t.isStopped() && !t.isPaused()) << "Timeline::stop() failed";

    ASSERT_EQ(luaRun(L, "t:rewind()"), 0) << luaGetError("Timeline::rewind() failed");
    ASSERT_TRUE(t.isRewinding() && !t.isPaused()) << "Timeline::rewind() failed";

    t.play(); t.pause();

    ASSERT_EQ(luaRun(L, "t:resume()"), 0) << luaGetError("Timeline::resume() failed");
    ASSERT_FALSE(t.isPaused()) << "Timeline::resume() failed";

    ASSERT_EQ(luaRun(L, "t:seek(0.25, true, true)"), 0) << luaGetError("Timeline::seek() failed");
    ASSERT_EQ(t.getCurrentTime(), 0.25f) << "Timeline::seek() failed";
    ASSERT_TRUE(t.isPaused() && t.isRewinding()) << "Timeline::seek() failed";

    t.seek(0.0f, false, false); t.stop();

    ASSERT_EQ(luaRun(L, "t:addTrack(\"test2\", 70.0, 80.0, \"QUAD_IN\")"), 0) << luaGetError("Timeline::addTrack() failed");
    ASSERT_EQ(t.getTracksCount(), 2) << "Timeline::addTrack() failed";
    ASSERT_STREQ(t.getTracki(1)->getName(), "test2") << "Timeline::addTrack() failed";
    ASSERT_EQ(t.getTrack("test2")->getValue(0.0f), 70.0f) << "Timeline::addTrack() failed";

    }

    lua_close(L);
}

TEST(LuaScripting, Dynamics)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Dynamics::lua_openDynamicsLib(L);

    { // Runtime scope

    Dynamics d;
    d.enabled = true;
    d.smoothers["test"] = Smoother();
    d.smoothers["test"].setValue(10.0f);
    d.timelines["timelineTest"] = Timeline();
    d.timelines["timelineTest"].addTrack("test", 10.0f, 20.0f);

    d.lua_this(L);
    lua_setglobal(L, "d");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(d).__name == 'Dynamics')"), 0) << luaGetError("Dynamics metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "d:isEnabled() == true"), 0) << luaGetError("Dynamics::isEnabled() failed");

    ASSERT_EQ(luaAssert(L, "d:getSmoother('test'):getValue() == 10.0"), 0) << luaGetError("Dynamics::getSmoother() failed");
    ASSERT_EQ(luaAssert(L, "d:getTimeline('timelineTest'):getValue('test') == 10.0"), 0) << luaGetError("Dynamics::getTimeline() failed");

    ASSERT_EQ(luaRun(L, "d:getSmoother('test2')"), 0) << luaGetError("Dynamics::getSmoother() failed");
    ASSERT_EQ(d.smoothers.count("test2"), 1) << "Dynamics::getSmoother() failed";

    ASSERT_EQ(luaRun(L, "d:deleteSmoother('test')"), 0) << luaGetError("Dynamics::deleteSmoother() failed");
    ASSERT_EQ(d.smoothers.count("test"), 0) << "Dynamics::deleteSmoother() failed";

    ASSERT_EQ(luaRun(L, "d:getTimeline('timelineTest2')"), 0) << luaGetError("Dynamics::getTimeline() failed");
    ASSERT_EQ(d.timelines.count("timelineTest2"), 1) << "Dynamics::getTimeline() failed";

    ASSERT_EQ(luaRun(L, "d:deleteTimeline('timelineTest')"), 0) << luaGetError("Dynamics::deleteTimeline() failed");
    ASSERT_EQ(d.timelines.count("timelineTest"), 0) << "Dynamics::deleteTimeline() failed";

    ASSERT_EQ(luaRun(L, "UpdateValue = 20.0"), 0) << luaGetError("Dynamics::update() failed");
    ASSERT_EQ(luaRun(L, "d.onUpdate = function(dt) UpdateValue = 30.0 end"), 0) << luaGetError("Dynamics::update() failed");
    d.update(0.0f);
    ASSERT_EQ(luaAssert(L, "UpdateValue == 30.0"), 0) << luaGetError("Dynamics::update() failed");

    Dynamics d2;
    d2.lua_this(L);
    lua_setglobal(L, "d2");

    ASSERT_EQ(luaRun(L, "d2.onUpdate = function(dt) UpdateValue = dt end"), 0) << luaGetError("Dynamics::update() failed");
    d2.update(50.0f);
    ASSERT_EQ(luaAssert(L, "UpdateValue == 50.0"), 0) << luaGetError("Dynamics::update() failed");

    }

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

    { // Runtime scope

    DummySceneObject o;
    o.getTransform().position = glm::vec3(10.0f, 20.0f, 30.0f);
    o.getDynamics().enabled = true;
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
    o.lua_this(L);
    lua_setglobal(L, "o2");
    o.lua_this(L);
    lua_setglobal(L, "o3");
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

    ASSERT_EQ(luaAssert(L, "o == o2"), 0) << luaGetError("SceneObject::operator==() failed");
    ASSERT_EQ(luaAssert(L, "o == o3"), 0) << luaGetError("SceneObject::operator==() failed");
    ASSERT_EQ(luaAssert(L, "o ~= c1"), 0) << luaGetError("SceneObject::operator==() failed");
    ASSERT_EQ(luaAssert(L, "o ~= c2"), 0) << luaGetError("SceneObject::operator==() failed");

    ASSERT_EQ(luaAssert(L, "o.transform:getPosition()[1] == 10.0"), 0) << luaGetError("SceneObject::transform.getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "o.transform:getPosition()[2] == 20.0"), 0) << luaGetError("SceneObject::transform.getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "o.transform:getPosition()[3] == 30.0"), 0) << luaGetError("SceneObject::transform.getPosition()[3] failed");
    ASSERT_EQ(luaAssert(L, "o.dynamics:isEnabled() == true"), 0) << luaGetError("SceneObject::dynamics.isEnabled() failed");
    ASSERT_EQ(luaAssert(L, "o:isDynamic() == true"), 0) << luaGetError("SceneObject::isDynamic() failed");

    ASSERT_EQ(luaRun(L, "o.dynamics.onUpdate = function(dt, obj) obj.transform:setPosition({1.0, 2.0, dt}) end"), 0) << luaGetError("SceneObject::update() failed");
    o.doUpdate(5.0f);
    ASSERT_EQ(o.getTransform().position, glm::vec3(1.0f, 2.0f, 5.0f)) << "SceneObject::update() failed";

    o.setDynamic(false);
    o.doUpdate(10.0f);
    ASSERT_EQ(o.getTransform().position, glm::vec3(1.0f, 2.0f, 5.0f)) << "SceneObject::update() failed";

    ASSERT_EQ(luaRun(L, "o:setName(\"TestObject2\")"), 0) << luaGetError("SceneObject::setName() failed");
    ASSERT_STREQ(o.getName().c_str(), "TestObject2") << "SceneObject::setName() failed";

    ASSERT_EQ(luaRun(L, "o:addChild(c3)"), 0) << luaGetError("SceneObject::addChild() failed");
    ASSERT_EQ(o.getChildCount(), 3) << "SceneObject::addChild() failed";

    ASSERT_EQ(luaRun(L, "o:removeChild(c2)"), 0) << luaGetError("SceneObject::removeChild() failed");
    ASSERT_EQ(o.getChildCount(), 2) << "SceneObject::removeChild() failed";

    ASSERT_EQ(luaRun(L, "o:removeAllChildren()"), 0) << luaGetError("SceneObject::removeAllChildren() failed");
    ASSERT_EQ(o.getChildCount(), 0) << "SceneObject::removeAllChildren() failed";

    ASSERT_EQ(luaRun(L, "o:setDynamic(true)"), 0) << luaGetError("SceneObject::setDynamic() failed");
    ASSERT_EQ(o.isDynamic(), true) << "SceneObject::setDynamic() failed";

    }

    lua_close(L);
}

TEST(LuaScripting, LightObject)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    LightObject::lua_openSceneObjectLib(L);

    { // Runtime scope

    LightObject l;
    l.getTransform().position = glm::vec3(10.0f, 20.0f, 30.0f);
    l.getDynamics().enabled = true;
    l.setColor(glm::vec3(40.0f, 50.0f, 60.0f));
    l.setIntensity(70.0f);

    l.lua_this(L);
    lua_setglobal(L, "l");
    l.lua_this(L);
    lua_setglobal(L, "l2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(l).__name == 'LightObject')"), 0) << luaGetError("LightObject metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "l:getType() == \"LIGHT\""), 0) << luaGetError("LightObject::getType() failed");

    ASSERT_EQ(luaAssert(L, "l == l2"), 0) << luaGetError("SceneObject::operator==() failed");

    ASSERT_EQ(luaAssert(L, "l.transform:getPosition()[1] == 10.0"), 0) << luaGetError("LightObject::transform.getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "l.transform:getPosition()[2] == 20.0"), 0) << luaGetError("LightObject::transform.getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "l.transform:getPosition()[3] == 30.0"), 0) << luaGetError("LightObject::transform.getPosition()[3] failed");
    ASSERT_EQ(luaAssert(L, "l.dynamics:isEnabled() == true"), 0) << luaGetError("LightObject::dynamics.isEnabled() failed");

    ASSERT_EQ(luaRun(L, "l.dynamics.onUpdate = function(dt, obj) obj.transform:setPosition({1.0, 2.0, dt}) end"), 0) << luaGetError("LightObject::update() failed");
    l.doUpdate(5.0f);
    ASSERT_EQ(l.getTransform().position, glm::vec3(1.0f, 2.0f, 5.0f)) << "LightObject::update() failed";

    ASSERT_EQ(luaAssert(L, "l:getColor()[1] == 40.0"), 0) << luaGetError("LightObject::getColor()[1] failed");
    ASSERT_EQ(luaAssert(L, "l:getColor()[2] == 50.0"), 0) << luaGetError("LightObject::getColor()[2] failed");
    ASSERT_EQ(luaAssert(L, "l:getColor()[3] == 60.0"), 0) << luaGetError("LightObject::getColor()[3] failed");

    ASSERT_EQ(luaAssert(L, "l:getIntensity() == 70.0"), 0) << luaGetError("LightObject::getIntensity() failed");

    ASSERT_EQ(luaRun(L, "l:setColor({1.0, 2.0, 3.0})"), 0) << luaGetError("LightObject::setColor() failed");
    ASSERT_EQ(l.getColor(), glm::vec3(1.0f, 2.0f, 3.0f)) << "LightObject::setColor() failed";

    ASSERT_EQ(luaRun(L, "l:setIntensity(4.0)"), 0) << luaGetError("LightObject::setIntensity() failed");
    ASSERT_EQ(l.getIntensity(), 4.0f) << "LightObject::setIntensity() failed";

    }

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

    { // Runtime scope

    PhongMaterial material;
    material.ambient = glm::vec3(10.0f, 20.0f, 30.0f);

    MeshObject m;
    m.getTransform().position = glm::vec3(40.0f, 50.0f, 60.0f);
    m.getDynamics().enabled = true;
    m.setMesh(mesh1);
    m.setMaterial(material);

    m.lua_this(L);
    lua_setglobal(L, "m");
    m.lua_this(L);
    lua_setglobal(L, "m2");
    luaPushSharedPtr<Mesh>(L, mesh2, "MeshPtr"); 
    lua_setglobal(L, "meshPtr2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(m).__name == 'MeshObject')"), 0) << luaGetError("MeshObject metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "m:getType() == \"MESH\""), 0) << luaGetError("MeshObject::getType() failed");

    ASSERT_EQ(luaAssert(L, "m == m2"), 0) << luaGetError("SceneObject::operator==() failed");

    ASSERT_EQ(luaAssert(L, "m.transform:getPosition()[1] == 40.0"), 0) << luaGetError("MeshObject::transform.getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "m.transform:getPosition()[2] == 50.0"), 0) << luaGetError("MeshObject::transform.getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "m.transform:getPosition()[3] == 60.0"), 0) << luaGetError("MeshObject::transform.getPosition()[3] failed");
    ASSERT_EQ(luaAssert(L, "m.dynamics:isEnabled() == true"), 0) << luaGetError("MeshObject::dynamics.isEnabled() failed");

    ASSERT_EQ(luaRun(L, "m.dynamics.onUpdate = function(dt, obj) obj.transform:setPosition({1.0, 2.0, dt}) end"), 0) << luaGetError("MeshObject::update() failed");
    m.doUpdate(5.0f);
    ASSERT_EQ(m.getTransform().position, glm::vec3(1.0f, 2.0f, 5.0f)) << "MeshObject::update() failed";

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

    }

    lua_close(L); 
}

TEST(LuaScripting, Camera)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    Camera::lua_openSceneObjectLib(L);

    { // Runtime scope

    Camera c;
    c.getTransform().position = glm::vec3(10.0f, 20.0f, 30.0f);
    c.getDynamics().enabled = true;
    c.projectionMode = Camera::ORTHOGRAPHIC;
    c.fovMode = Camera::HORIZONTAL;
    c.fieldOfView = 40.0f;
    c.orthographicSize = 50.0f;
    c.nearPlane = 60.0f;
    c.farPlane = 70.0f;
    c.perspectiveShift = glm::vec2(80.0f, 90.0f);

    c.lua_this(L);
    lua_setglobal(L, "c");
    c.lua_this(L);
    lua_setglobal(L, "c2");

    ASSERT_EQ(luaRun(L, "assert(getmetatable(c).__name == 'Camera')"), 0) << luaGetError("Camera metatable name assertion failed");

    ASSERT_EQ(luaAssert(L, "c:getType() == \"CAMERA\""), 0) << luaGetError("Camera::getType() failed");

    ASSERT_EQ(luaAssert(L, "c == c2"), 0) << luaGetError("SceneObject::operator==() failed");

    ASSERT_EQ(luaAssert(L, "c.transform:getPosition()[1] == 10.0"), 0) << luaGetError("Camera::transform.getPosition()[1] failed");
    ASSERT_EQ(luaAssert(L, "c.transform:getPosition()[2] == 20.0"), 0) << luaGetError("Camera::transform.getPosition()[2] failed");
    ASSERT_EQ(luaAssert(L, "c.transform:getPosition()[3] == 30.0"), 0) << luaGetError("Camera::transform.getPosition()[3] failed");
    ASSERT_EQ(luaAssert(L, "c.dynamics:isEnabled() == true"), 0) << luaGetError("Camera::dynamics.isEnabled() failed");

    ASSERT_EQ(luaRun(L, "c.dynamics.onUpdate = function(dt, obj) obj.transform:setPosition({1.0, 2.0, dt}) end"), 0) << luaGetError("Camera::update() failed");
    c.doUpdate(5.0f);
    ASSERT_EQ(c.getTransform().position, glm::vec3(1.0f, 2.0f, 5.0f)) << "Camera::update() failed";

    ASSERT_EQ(luaAssert(L, "c:getProjectionMode() == \"ORTHOGRAPHIC\""), 0) << luaGetError("Camera::getProjectionMode() failed");

    ASSERT_EQ(luaAssert(L, "c:getFOVMode() == \"HORIZONTAL\""), 0) << luaGetError("Camera::getFOVMode() failed");

    ASSERT_EQ(luaAssert(L, "c:getOrthographicSize() == 50.0"), 0) << luaGetError("Camera::getOrthographicSize() failed");

    ASSERT_EQ(luaAssert(L, "c:getFieldOfView() == 40.0"), 0) << luaGetError("Camera::getFieldOfView() failed");

    ASSERT_EQ(luaAssert(L, "c:getNearPlane() == 60.0"), 0) << luaGetError("Camera::getNearPlane() failed");

    ASSERT_EQ(luaAssert(L, "c:getFarPlane() == 70.0"), 0) << luaGetError("Camera::getFarPlane() failed");

    ASSERT_EQ(luaAssert(L, "c:getPerspectiveShift()[1] == 80.0"), 0) << luaGetError("Camera::getPerspectiveShift()[1] failed");
    ASSERT_EQ(luaAssert(L, "c:getPerspectiveShift()[2] == 90.0"), 0) << luaGetError("Camera::getPerspectiveShift()[2] failed");

    ASSERT_EQ(luaRun(L, "c:setProjectionMode(\"PERSPECTIVE\")"), 0) << luaGetError("Camera::setProjectionMode() failed");
    ASSERT_EQ(c.projectionMode, Camera::PERSPECTIVE) << "Camera::setProjectionMode() failed";

    ASSERT_EQ(luaRun(L, "c:setFOVMode(\"VERTICAL\")"), 0) << luaGetError("Camera::setFOVMode() failed");
    ASSERT_EQ(c.fovMode, Camera::VERTICAL) << "Camera::setFOVMode() failed";

    ASSERT_EQ(luaRun(L, "c:setOrthographicSize(100.0)"), 0) << luaGetError("Camera::setOrthographicSize() failed");
    ASSERT_EQ(c.orthographicSize, 100.0f) << "Camera::setOrthographicSize() failed";

    ASSERT_EQ(luaRun(L, "c:setFieldOfView(110.0)"), 0) << luaGetError("Camera::setFieldOfView() failed");
    ASSERT_EQ(c.fieldOfView, 110.0f) << "Camera::setFieldOfView() failed";

    ASSERT_EQ(luaRun(L, "c:setNearPlane(120.0)"), 0) << luaGetError("Camera::setNearPlane() failed");
    ASSERT_EQ(c.nearPlane, 120.0f) << "Camera::setNearPlane() failed";

    ASSERT_EQ(luaRun(L, "c:setFarPlane(130.0)"), 0) << luaGetError("Camera::setFarPlane() failed");
    ASSERT_EQ(c.farPlane, 130.0f) << "Camera::setFarPlane() failed";

    ASSERT_EQ(luaRun(L, "c:setPerspectiveShift({140.0, 150.0})"), 0) << luaGetError("Camera::setPerspectiveShift() failed");
    ASSERT_EQ(c.perspectiveShift, glm::vec2(140.0f, 150.0f)) << "Camera::setPerspectiveShift() failed";

    }

    lua_close(L);
}