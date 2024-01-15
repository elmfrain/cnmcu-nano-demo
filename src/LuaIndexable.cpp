#include "LuaInclude.hpp"
#include "animation/Track.hpp"

template<>
const char* LuaIndexable<em::Keyframe>::luaIndexTableName()
{
    return "_KeyframeInstances";
}

template<>
const char* LuaIndexable<em::Track>::luaIndexTableName()
{
    return "_TrackInstances";
}