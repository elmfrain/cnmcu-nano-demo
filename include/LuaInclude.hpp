#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define luaGet(var, varType, type, index) \
    if(!lua_is##type(L, index)) \
        return luaL_error(L, "Expected " #type " at index %d", index); \
    var = static_cast<varType>(lua_to##type(L, index));\
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
