#include "animation/Track.hpp"

#include <algorithm>
#include <cstring>
#include <cstdio>

using namespace em;

#define luaGetKeyframe() \
    Keyframe* keyframe = nullptr; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(keyframe, Keyframe, -1); \

int Keyframe::lua_this(lua_State* L)
{
    if(luaIndexable.hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "Keyframe");
    lua_setmetatable(L, -2);

    luaIndexable.luaRegisterInstance(L);

    return 1;
}

int Keyframe::lua_openKeyframeLib(lua_State* L)
{
    luaL_Reg keyframeMethods[] =
    {
        { "getTime", lua_getTime },
        { "getValue", lua_getValue },
        { "getEasing", lua_getEasing },
        { "setTime", lua_setTime },
        { "setValue", lua_setValue },
        { "setEasing", lua_setEasing },
        { nullptr, nullptr }
    };

    luaL_newmetatable(L, "Keyframe");
    luaL_setfuncs(L, keyframeMethods, 0);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_setglobal(L, "Keyframe");

    LuaIndexable<Keyframe>::luaRegisterType(L);

    return 1;
}

int Keyframe::lua_getTime(lua_State* L)
{
    luaGetKeyframe();
    lua_pushnumber(L, keyframe->time);
    return 1;
}

int Keyframe::lua_getValue(lua_State* L)
{
    luaGetKeyframe();
    lua_pushnumber(L, keyframe->value);
    return 1;
}

int Keyframe::lua_getEasing(lua_State* L)
{
    luaGetKeyframe();
    lua_pushstring(L, Easing::getEasingFunctionName(keyframe->easing));
    return 1;
}

int Keyframe::lua_setTime(lua_State* L)
{
    luaGetKeyframe();
    keyframe->time = static_cast<float>(luaL_checknumber(L, 2));
    return 0;
}

int Keyframe::lua_setValue(lua_State* L)
{
    luaGetKeyframe();
    keyframe->value = static_cast<float>(luaL_checknumber(L, 2));
    return 0;
}

int Keyframe::lua_setEasing(lua_State* L)
{
    luaGetKeyframe();
    keyframe->easing = Easing::getEasingFunction(luaL_checkstring(L, 2));
    return 0;
}

// -----------------------------------------------------------------------------------------------------------------------------------
// Track
// -----------------------------------------------------------------------------------------------------------------------------------

Track::Track()
{
    memset(this->name, 0, 32);
    snprintf(this->name, 31, "Track %p", this);

    addKeyframe(0.0f, 0.0f);
    addKeyframe(1.0f, 0.0f);
}

Track::Track(const char* name)
{
    memset(this->name, 0, 32);
    strncpy(this->name, name, 31);

    addKeyframe(0.0f, 0.0f);
    addKeyframe(1.0f, 0.0f);
}

Track::Track(const char* name, float value)
{
    memset(this->name, 0, 32);
    strncpy(this->name, name, 31);

    addKeyframe(0.0f, value);
    addKeyframe(1.0f, value);
}

Track::Track(const char* name, std::vector<Keyframe>&& keyframes)
{
    memset(this->name, 0, 32);
    strncpy(this->name, name, 31);

    addKeyframes(std::move(keyframes));

    if (keyframesCount == 0)
    {
        addKeyframe(0.0f, 0.0f);
        addKeyframe(1.0f, 0.0f);
    }
    else if (keyframesCount == 1)
        addKeyframe(1.0f, keyframes[0].value);
}

Track::Track(const char* name, float startValue, float endValue, Easing::Function easing)
{
    memset(this->name, 0, 32);
    strncpy(this->name, name, 31);

    addKeyframe(0.0f, startValue, easing);
    addKeyframe(1.0f, endValue);
}

Track::Track(Track&& other)
{
    *this = std::move(other);
}

Track& Track::operator=(Track&& other)
{
    if(this == &other)
        return *this;

    memcpy(name, other.name, 32);
    keyframesCount = other.keyframesCount;
    keyframesSmallList = std::move(other.keyframesSmallList);
    keyframesLargeList = std::move(other.keyframesLargeList);

    if(keyframesCount < 16)
        keyframes = keyframesSmallList.data();
    else
        keyframes = keyframesLargeList.data();

    other.keyframes = other.keyframesSmallList.data();
    other.keyframesCount = 0;
    other.currentKeyframe = nullptr;
    other.nextKeyframe = nullptr;

    return *this;
}

void Track::addKeyframe(Keyframe&& keyframe)
{
    for(size_t i = 0; i < keyframesCount; i++)
        if (keyframes[i].time == keyframe.time)
        {
            keyframes[i] = std::move(keyframe);
            return;
        }

    if (keyframesCount == 16)
    {
        keyframesLargeList.reserve(16);
        for (size_t i = 0; i < 16; i++)
            keyframesLargeList.push_back(std::move(keyframesSmallList[i]));
    }

    if(keyframesCount < 16)
    {
        keyframes = keyframesSmallList.data();
        keyframes[keyframesCount] = std::move(keyframe);
    }
    else
    {
        keyframesLargeList.push_back(std::move(keyframe));
        keyframes = keyframesLargeList.data();
    }

    keyframesCount++;
}

void Track::addKeyframe(float time, float value, Easing::Function easing)
{
    addKeyframe({ time, value, easing });

    sortKeyframes();
}

void Track::addKeyframes(std::vector<Keyframe>&& keyframes)
{
    for (auto& keyframe : keyframes)
        addKeyframe(std::move(keyframe));

    sortKeyframes();
}

float Track::getValue(float time) const
{
    if (keyframesCount == 0)
        return 0.0f;

    if (keyframesCount == 1)
        return keyframes[0].value;

    findKeyframes(time);

    if (currentKeyframe == nextKeyframe)
        return currentKeyframe->value;

    float t = (time - currentKeyframe->time) / (nextKeyframe->time - currentKeyframe->time);
    return currentKeyframe->value + (nextKeyframe->value - currentKeyframe->value) * currentKeyframe->easing(t);
}

const char* Track::getName() const
{
    return name;
}

size_t Track::getKeyframesCount() const
{
    return keyframesCount;
}

int Track::lua_this(lua_State* L)
{
    if(hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "Track");
    lua_setmetatable(L, -2);

    luaRegisterInstance(L);

    return 1;
}

int Track::lua_openTrackLib(lua_State* L)
{
    luaL_Reg trackMethods[] =
    {
        { "getName", lua_getName },
        { "getKeyframesCount", lua_getKeyframesCount },
        { "getValue", lua_getValue },
        { "addKeyframe", lua_addKeyframe },
        { nullptr, nullptr }
    };

    luaL_newmetatable(L, "Track");
    luaL_setfuncs(L, trackMethods, 0);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_setglobal(L, "Track");

    LuaIndexable<Track>::luaRegisterType(L);

    Keyframe::lua_openKeyframeLib(L);

    return 1;
}

void Track::sortKeyframes()
{
    // std::sort(keyframes.begin(), keyframes.end(), [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
    std::sort(keyframes, keyframes + keyframesCount, [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
}

void Track::findKeyframes(float time) const
{
    if (time <= keyframes[0].time)
    {
        currentKeyframe = &keyframes[0];
        nextKeyframe = &keyframes[0];
        return;
    }

    if (time >= keyframes[keyframesCount - 1].time)
    {
        currentKeyframe = &keyframes[keyframesCount - 1];
        nextKeyframe = &keyframes[keyframesCount - 1];
        return;
    }

    for (size_t i = 0; i < keyframesCount - 1; i++)
    {
        if (time >= keyframes[i].time && time < keyframes[i + 1].time)
        {
            currentKeyframe = &keyframes[i];
            nextKeyframe = &keyframes[i + 1];
            return;
        }
    }
}

#define luaGetTrack() \
    Track* track = nullptr; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(track, Track, -1);

int Track::lua_getName(lua_State* L)
{
    luaGetTrack();
    lua_pushstring(L, track->getName());
    return 1;
}

int Track::lua_getKeyframesCount(lua_State* L)
{
    luaGetTrack();
    lua_pushinteger(L, track->getKeyframesCount());
    return 1;
}

int Track::lua_getValue(lua_State* L)
{
    luaGetTrack();
    lua_pushnumber(L, track->getValue(static_cast<float>(luaL_checknumber(L, 2))));
    return 1;
}

int Track::lua_addKeyframe(lua_State* L)
{
    luaGetTrack();
    if (lua_istable(L, 2))
    {
        luaPushValueFromKey("time", 2);
        luaPushValueFromKey("value", 2);
        luaPushValueFromKey("easing", 2);
        track->addKeyframe(static_cast<float>(luaL_checknumber(L, -3)), static_cast<float>(luaL_checknumber(L, -2)), Easing::getEasingFunction(luaL_checkstring(L, -1)));
    }
    else if(lua_gettop(L) == 4)
        track->addKeyframe(static_cast<float>(luaL_checknumber(L, 2)), static_cast<float>(luaL_checknumber(L, 3)), Easing::getEasingFunction(luaL_checkstring(L, 4)));
    else
        track->addKeyframe(static_cast<float>(luaL_checknumber(L, 2)), static_cast<float>(luaL_checknumber(L, 3)));

    return 0;
}