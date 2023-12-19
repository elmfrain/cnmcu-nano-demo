#include <LightObject.hpp>

using namespace em;

LightObject::LightObject(const std::string& name)
    : SceneObject(LIGHT, name)
    , color(1.0f, 1.0f, 1.0f)
    , intensity(1.0f)
{
}

LightObject::~LightObject()
{
}

void LightObject::update(float dt)
{
}

void LightObject::draw(Shader& shader)
{
}

void LightObject::setColor(const glm::vec3& color)
{
    this->color = color;
}

glm::vec3 LightObject::getColor() const
{
    return color;
}

void LightObject::setIntensity(float intensity)
{
    this->intensity = intensity;
}

float LightObject::getIntensity() const
{
    return intensity;
}

#define luaGetLightObject() \
    int error; \
    LightObject* lightObject; \
    if ((error = lua_getLightObject(L, &lightObject)) != 0) \
        return error; 

int LightObject::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);
    lua_pushstring(L, "transform");
    getTransform().lua_this(L);
    lua_settable(L, -3);

    lua_getglobal(L, "LightObject");
    lua_setmetatable(L, -2);

    return 1;
}

int LightObject::lua_openLightObjectLib(lua_State* L)
{
    static const luaL_Reg lightObjectLib[] =
    {
        { "getColor", lua_getColor },
        { "getIntensity", lua_getIntensity },
        { "setColor", lua_setColor },
        { "setIntensity", lua_setIntensity },
        { NULL, NULL }
    };

    luaL_newlib(L, lightObjectLib);
    lua_setglobal(L, "LightObject");

    lua_getglobal(L, "LightObject");
    lua_pushstring(L, "__index");
    lua_getglobal(L, "LightObject");
    lua_settable(L, -3);
    lua_getglobal(L, "SceneObject");
    lua_setmetatable(L, -2);
    lua_pop(L, 1);

    return 0;
}

int LightObject::lua_getLightObject(lua_State* L, LightObject** lightObject)
{
    luaPushValueFromKey("ptr", 1);
    luaGet(*lightObject, LightObject*, userdata, -1);

    return 0;
}

int LightObject::lua_getColor(lua_State* L)
{
    luaGetLightObject();
    luaPushVec3(lightObject->color);

    return 1;
}

int LightObject::lua_getIntensity(lua_State* L)
{
    luaGetLightObject();
    lua_pushnumber(L, lightObject->intensity);

    return 1;
}

int LightObject::lua_setColor(lua_State* L)
{
    luaGetLightObject();
    luaGetVec3(lightObject->color, 2);

    return 0;
}

int LightObject::lua_setIntensity(lua_State* L)
{
    luaGetLightObject();
    lightObject->intensity = lua_tonumber(L, 2);

    return 0;
}