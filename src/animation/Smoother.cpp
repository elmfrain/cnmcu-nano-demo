#include "animation/Smoother.hpp"

#include "glm/glm.hpp"

using namespace em;

Smoother::Smoother()
    : m_value(0.0f)
    , m_target(0.0f)
    , m_speed(10.0f)
    , m_damping(1.0f)
    , m_acceleration(0.0f)
    , m_velocity(0.0f)
    , m_grabbed(false)
    , m_springing(false)
{
}

void Smoother::grab()
{
    m_grabbed = true;
}

void Smoother::grab(float value)
{
    m_target = value;
    m_grabbed = true;
}

void Smoother::setValueAndGrab(float value)
{
    m_value = value;
    m_target = value;
    m_grabbed = true;
}

void Smoother::release()
{
    m_grabbed = false;
}

bool Smoother::isGrabbed() const
{
    return m_grabbed;
}

bool Smoother::isSpringing() const
{
    return m_springing;
}

void Smoother::setSpringing(bool springing)
{
    m_springing = springing;
}

void Smoother::setSpeed(float speed)
{
    m_speed = speed;
}

void Smoother::setDamping(float damping)
{
    m_damping = damping;
}

void Smoother::setValue(float value)
{
    m_value = value;
}

float Smoother::getValue() const
{
    return m_value;
}

float Smoother::getTarget() const
{
    return m_target;
}

float Smoother::getSpeed() const
{
    return m_speed;
}

float Smoother::getDamping() const
{
    return m_damping;
}

void Smoother::update(float delta)
{
    // Prevent jerkiness from high delta
    if (delta > 0.2f) delta = 0.2f;

    if (0.0f < delta)
    {
        if(m_springing)
        {
            m_acceleration = (m_target - m_value) * m_speed * 32.0f;
            m_velocity += m_acceleration * delta;
            m_velocity *= glm::pow(0.0025f / m_speed, delta);
        }
        else m_velocity = (m_target - m_value) * m_speed;

        m_value += m_velocity * delta;
        m_velocity *= glm::pow(0.0625f / (m_speed * m_damping), delta);
    }
}

#define luaGetSmoother() \
    Smoother* smoother; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(smoother, Smoother, -1);

int Smoother::lua_this(lua_State* L)
{
    if(hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "Smoother");
    lua_setmetatable(L, -2);

    luaRegisterInstance(L);

    return 1;
}

int Smoother::lua_openSmootherLib(lua_State* L)
{
    static const luaL_Reg luaSmootherMethods[] =
    {
        { "getValue", lua_getValue },
        { "getTarget", lua_getTarget },
        { "getSpeed", lua_getSpeed },
        { "getDamping", lua_getDamping },

        { "setValue", lua_setValue },
        { "setSpringing", lua_setSpringing },
        { "setSpeed", lua_setSpeed },
        { "setDamping", lua_setDamping },

        { "grab", lua_grab },
        { "release", lua_release },
        { "setValueAndGrab", lua_setValueAndGrab },

        { NULL, NULL }
    };

    luaL_newmetatable(L, "Smoother");
    luaL_setfuncs(L, luaSmootherMethods, 0);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_setglobal(L, "Smoother");

    LuaIndexable<Smoother>::luaRegisterType(L);

    return 0;
}

int Smoother::lua_getValue(lua_State* L)
{
    luaGetSmoother();
    lua_pushnumber(L, smoother->getValue());
    return 1;
}

int Smoother::lua_getTarget(lua_State* L)
{
    luaGetSmoother();
    lua_pushnumber(L, smoother->getTarget());
    return 1;
}

int Smoother::lua_getSpeed(lua_State* L)
{
    luaGetSmoother();
    lua_pushnumber(L, smoother->getSpeed());
    return 1;
}

int Smoother::lua_getDamping(lua_State* L)
{
    luaGetSmoother();
    lua_pushnumber(L, smoother->getDamping());
    return 1;
}

int Smoother::lua_setValue(lua_State* L)
{
    luaGetSmoother();
    luaGet(smoother->m_value, float, number, 2);
    return 0;
}

int Smoother::lua_setSpringing(lua_State* L)
{
    luaGetSmoother();
    luaGet(smoother->m_springing, bool, boolean, 2);
    return 0;
}

int Smoother::lua_setSpeed(lua_State* L)
{
    luaGetSmoother();
    luaGet(smoother->m_speed, float, number, 2);
    return 0;
}

int Smoother::lua_setDamping(lua_State* L)
{
    luaGetSmoother();
    luaGet(smoother->m_damping, float, number, 2);
    return 0;
}

int Smoother::lua_grab(lua_State* L)
{
    luaGetSmoother();

    if (lua_gettop(L) == 2)
    {
        float value;
        luaGet(value, float, number, 2);
        smoother->grab(value);
    }
    else smoother->grab();

    return 0;
}

int Smoother::lua_release(lua_State* L)
{
    luaGetSmoother();
    smoother->release();
    return 0;
}

int Smoother::lua_setValueAndGrab(lua_State* L)
{
    luaGetSmoother();
    
    float value;
    luaGet(value, float, number, 2);
    smoother->setValueAndGrab(value);
    return 0;
}