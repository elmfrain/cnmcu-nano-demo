#include "LuaInclude.hpp"
#include "animation/Track.hpp"
#include "animation/Timeline.hpp"

template<>
const char* LuaIndexable<em::Keyframe>::luaIndexTableName()
{
    return "__KeyframeInstances";
}

template<>
const char* LuaIndexable<em::Track>::luaIndexTableName()
{
    return "__TrackInstances";
}

template<>
const char* LuaIndexable<em::Timeline>::luaIndexTableName()
{
    return "__TimelineInstances";
}