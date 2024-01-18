#include "LuaInclude.hpp"
#include "animation/Track.hpp"
#include "animation/Timeline.hpp"
#include "animation/Smoother.hpp"
#include "SceneObject.hpp"

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

template<>
const char* LuaIndexable<em::Smoother>::luaIndexTableName()
{
    return "__SmootherInstances";
}

template<>
const char* LuaIndexable<em::Dynamics>::luaIndexTableName()
{
    return "__DynamicsInstances";
}