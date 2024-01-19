#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <memory>

#define luaGet(var, varType, type, index) \
    if(!lua_is##type(L, index)) \
        return luaL_error(L, "Expected " #type " at index %d", index); \
    var = static_cast<varType>(lua_to##type(L, index));\
    if(index < 0) \
        lua_pop(L, 1);

#define luaGetPointer(var, varType, index) \
    if(!lua_islightuserdata(L, index)) \
        return luaL_error(L, "Expected light userdata at index %d", index); \
    var = static_cast<varType*>(lua_touserdata(L, index)); \
    if(index < 0) \
        lua_pop(L, 1);

#define luaGetVec2(var, index) \
    if(!lua_istable(L, index)) \
        return luaL_error(L, "Expected table at index %d", index); \
    if(lua_rawlen(L, index) != 2) \
        return luaL_error(L, "Expected table of length 2 at index %d", index); \
    lua_rawgeti(L, index, 1); \
    lua_rawgeti(L, index < 0 ? index - 1 : index, 2); \
    var.x = lua_tonumber(L, -2); \
    var.y = lua_tonumber(L, -1); \
    lua_pop(L, 2); \
    if(index < 0) \
        lua_pop(L, 1);

#define luaGetVec3(var, index) \
    if(!lua_istable(L, index)) \
        return luaL_error(L, "Expected table at index %d", index); \
    if(lua_rawlen(L, index) != 3) \
        return luaL_error(L, "Expected table of length 3 at index %d", index); \
    lua_rawgeti(L, index, 1); \
    lua_rawgeti(L, index < 0 ? index - 1 : index, 2); \
    lua_rawgeti(L, index < 0 ? index - 2 : index, 3); \
    var.x = lua_tonumber(L, -3); \
    var.y = lua_tonumber(L, -2); \
    var.z = lua_tonumber(L, -1); \
    lua_pop(L, 3); \
    if(index < 0) \
        lua_pop(L, 1);

#define luaGetVec4(var, index) \
    if(!lua_istable(L, index)) \
        return luaL_error(L, "Expected table at index %d", index); \
    if(lua_rawlen(L, index) != 4) \
        return luaL_error(L, "Expected table of length 4 at index %d", index); \
    lua_rawgeti(L, index, 1); \
    lua_rawgeti(L, index < 0 ? index - 1 : index, 2); \
    lua_rawgeti(L, index < 0 ? index - 2 : index, 3); \
    lua_rawgeti(L, index < 0 ? index - 3 : index, 4); \
    var.x = lua_tonumber(L, -4); \
    var.y = lua_tonumber(L, -3); \
    var.z = lua_tonumber(L, -2); \
    var.w = lua_tonumber(L, -1); \
    lua_pop(L, 4); \
    if(index < 0) \
        lua_pop(L, 1);

#define luaGetQuat(var, index) \
    if(!lua_istable(L, index)) \
        return luaL_error(L, "Expected table at index %d", index); \
    if(lua_rawlen(L, index) != 4) \
        return luaL_error(L, "Expected table of length 4 at index %d", index); \
    lua_rawgeti(L, index, 1); \
    lua_rawgeti(L, index < 0 ? index - 1 : index, 2); \
    lua_rawgeti(L, index < 0 ? index - 2 : index, 3); \
    lua_rawgeti(L, index < 0 ? index - 3 : index, 4); \
    var.w = lua_tonumber(L, -4); \
    var.x = lua_tonumber(L, -3); \
    var.y = lua_tonumber(L, -2); \
    var.z = lua_tonumber(L, -1); \
    lua_pop(L, 4); \
    if(index < 0) \
        lua_pop(L, 1);

#define luaPushVec2(var) \
    lua_newtable(L); \
    lua_pushnumber(L, var.x); \
    lua_rawseti(L, -2, 1); \
    lua_pushnumber(L, var.y); \
    lua_rawseti(L, -2, 2);

#define luaPushVec3(var) \
    lua_newtable(L); \
    lua_pushnumber(L, var.x); \
    lua_rawseti(L, -2, 1); \
    lua_pushnumber(L, var.y); \
    lua_rawseti(L, -2, 2); \
    lua_pushnumber(L, var.z); \
    lua_rawseti(L, -2, 3);

#define luaPushVec4(var) \
    lua_newtable(L); \
    lua_pushnumber(L, var.x); \
    lua_rawseti(L, -2, 1); \
    lua_pushnumber(L, var.y); \
    lua_rawseti(L, -2, 2); \
    lua_pushnumber(L, var.z); \
    lua_rawseti(L, -2, 3); \
    lua_pushnumber(L, var.w); \
    lua_rawseti(L, -2, 4);

#define luaPushQuat(var) \
    lua_newtable(L); \
    lua_pushnumber(L, var.w); \
    lua_rawseti(L, -2, 1); \
    lua_pushnumber(L, var.x); \
    lua_rawseti(L, -2, 2); \
    lua_pushnumber(L, var.y); \
    lua_rawseti(L, -2, 3); \
    lua_pushnumber(L, var.z); \
    lua_rawseti(L, -2, 4);

#define luaPushValueFromKey(key, index) \
    if(!lua_istable(L, index)) \
        return luaL_error(L, "Expected table at index %d", index); \
    lua_pushstring(L, key); \
    lua_gettable(L, index < 0 ? index - 1 : index); \
    if(lua_isnoneornil(L, -1)) \
        return luaL_error(L, "Expected key %s at index %d", key, index);

static bool luaHasKeyValue(lua_State* L, const char* key, int index)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);
    bool hasKey = !lua_isnoneornil(L, -1);
    lua_pop(L, 1);
    return hasKey;
}

template<typename T>
int luaPushSharedPtr(lua_State* L, std::shared_ptr<T> ptr, const char* typeName) // typeName is used so that RTTI is not required
{
    std::shared_ptr<T>* userdata = static_cast<std::shared_ptr<T>*>(lua_newuserdata(L, sizeof(std::shared_ptr<T>)));
    new(userdata) std::shared_ptr<T>(ptr);

    luaL_newmetatable(L, typeName);
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, [](lua_State* L) -> int
    {
        std::shared_ptr<T>* userdata = static_cast<std::shared_ptr<T>*>(lua_touserdata(L, 1));
        userdata->~shared_ptr();
        return 0;
    });
    lua_settable(L, -3);

    lua_setmetatable(L, -2);

    return 1;
}

template<typename T>
int luaGetSharedPtr(lua_State* L, std::shared_ptr<T>& ptr, const char* typeName, int index = 1)
{
    if(!lua_isuserdata(L, index))
    {
        int otherType = lua_type(L, index);
        luaL_error(L, "Expected userdata at index %d, got %s", index, lua_typename(L, otherType));
        return 0;
    }

    std::shared_ptr<T>* userdata = static_cast<std::shared_ptr<T>*>(lua_touserdata(L, index));
    if(!userdata)
        return luaL_error(L, "Expected userdata at index %d", index);

    lua_pop(L, 1);

    ptr = *userdata;

    return 0;
}

template<typename T>
class LuaIndexable
{
private:
    int m_Id;
    lua_State* L = nullptr;

    static int nextId;
    static const char* luaIndexTableName(); // defined in LuaIndexable.cpp
public:
    LuaIndexable()
    {
        m_Id = -1;
    }

    ~LuaIndexable()
    {
        if(!L) return;

        lua_getglobal(L, luaIndexTableName());
        lua_pushinteger(L, m_Id);
        lua_pushnil(L);
        lua_settable(L, -3);
        lua_pop(L, 1);
    }

    LuaIndexable(const LuaIndexable& other)
    {
        *this = other;
    }

    LuaIndexable& operator=(const LuaIndexable& other)
    {
        m_Id = other.m_Id;
        L = other.L;

        return *this;
    }

    LuaIndexable(LuaIndexable&& other)
    {
        *this = std::move(other);
    }

    LuaIndexable& operator=(LuaIndexable&& other)
    {
        m_Id = other.m_Id;
        L = other.L;
        other.L = nullptr;

        return *this;
    }

    static void luaRegisterType(lua_State* L)
    {
        lua_getglobal(L, luaIndexTableName());
        if(!lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            return;
        }
        lua_pop(L, 1);

        lua_newtable(L);
        lua_setglobal(L, luaIndexTableName());
    }

    int hasLuaInstance(lua_State* L)
    {
        if(m_Id < 0)
            return false;

        lua_getglobal(L, luaIndexTableName());
        lua_pushinteger(L, m_Id);
        lua_gettable(L, -2);
        bool hasInstance = !lua_isnil(L, -1);

        if(hasInstance) lua_remove(L, -2);
        else lua_pop(L, 2);

        this->L = L;

        return hasInstance;
    }

    void luaRegisterInstance(lua_State* L)
    {
        m_Id = nextId++;

        lua_getglobal(L, luaIndexTableName());
        lua_pushinteger(L, m_Id);
        lua_pushvalue(L, -3);
        lua_settable(L, -3);
        lua_pop(L, 1);

        this->L = L;
    }
};

template<typename T>
int LuaIndexable<T>::nextId = 0;