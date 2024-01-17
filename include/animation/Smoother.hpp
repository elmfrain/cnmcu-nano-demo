#pragma once

#include "LuaInclude.hpp"

namespace em
{
    class Smoother : private LuaIndexable<Smoother>
    {
    public:
        Smoother();

        void grab();
        void grab(float value);
        void setValueAndGrab(float value);
        void release();

        bool isGrabbed() const;
        bool isSpringing() const;

        void setSpringing(bool springing);
        void setSpeed(float speed);
        void setDamping(float damping);
        void setValue(float value);

        float getValue() const;
        float getTarget() const;
        float getSpeed() const;
        float getDamping() const;

        void update(float dt);

        int lua_this(lua_State* L);
        static int lua_openSmootherLib(lua_State* L);
    private:
        float m_value;
        float m_target;
        float m_speed;
        float m_damping;

        float m_acceleration;
        float m_velocity;

        bool m_grabbed;
        bool m_springing;

        static int lua_getValue(lua_State* L);
        static int lua_getTarget(lua_State* L);
        static int lua_getSpeed(lua_State* L);
        static int lua_getDamping(lua_State* L);

        static int lua_setValue(lua_State* L);
        static int lua_setSpringing(lua_State* L);
        static int lua_setSpeed(lua_State* L);
        static int lua_setDamping(lua_State* L);

        static int lua_grab(lua_State* L);
        static int lua_release(lua_State* L);
        static int lua_setValueAndGrab(lua_State* L);
    };
}