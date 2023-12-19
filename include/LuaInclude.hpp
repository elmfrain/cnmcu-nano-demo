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

#define luaPushValueFromKey(key, index) \
    lua_pushstring(L, key); \
    lua_gettable(L, index); \
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
        return luaL_error(L, "Expected userdata at index %d", index);

    std::shared_ptr<T>* userdata = static_cast<std::shared_ptr<T>*>(lua_touserdata(L, index));
    if(!userdata)
        return luaL_error(L, "Expected userdata at index %d", index);

    luaL_getmetatable(L, typeName);
    if(!lua_getmetatable(L, index))
        return luaL_error(L, "Expected userdata at index %d", index);

    if(!lua_rawequal(L, -1, -2))
        return luaL_error(L, "Expected userdata at index %d", index);

    lua_pop(L, 2);

    ptr = *userdata;

    return 0;
}