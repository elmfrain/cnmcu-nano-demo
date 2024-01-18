#include "animation/Timeline.hpp"

#include <glm/glm.hpp>

using namespace em;

Timeline::Timeline()
{
}

Timeline::Timeline(std::vector<Track>&& tracks)
{
    addTracks(std::move(tracks));
}

Timeline::Timeline(Timeline&& other)
{
    *this = std::move(other);
}

Timeline& Timeline::operator=(Timeline&& other)
{
    tracks = std::move(other.tracks);
    tracksMap = std::move(other.tracksMap);
    tracksCount = other.tracksCount;

    if(tracksCount <= 8)
        tracks = tracksSmallList.data();
    else
        tracks = tracksLargeList.data();

    duration = other.duration;
    currentTime = other.currentTime;
    speed = other.speed;
    paused = other.paused;
    stopped = other.stopped;
    rewinding = other.rewinding;
    looping = other.looping;

    return *this;
}

void Timeline::addTrack(Track&& track)
{
    // Replace track if it already exists
    if (tracksMap.count(track.getName()) > 0)
    {
        tracks[tracksMap[track.getName()]] = std::move(track);
        return;
    }

    if (tracksCount == 8)
    {
        tracksLargeList.reserve(8);
        for (Track& track : tracksLargeList)
            tracksLargeList.push_back(std::move(track));
    }

    if (tracksCount < 8)
    {
        tracks = tracksSmallList.data();
        tracks[tracksCount] = std::move(track);
    }
    else
    {
        tracksLargeList.push_back(std::move(track));
        tracks = tracksLargeList.data();
    }

    tracksMap[track.getName()] = tracksCount++;
}

void Timeline::addTrack(const char* name, float value)
{
    addTrack(Track(name, value));
}

void Timeline::addTrack(const char* name, std::vector<Keyframe>&& keyframes)
{
    addTrack(Track(name, std::move(keyframes)));
}

void Timeline::addTrack(const char* name, float startValue, float endValue, Easing::Function easing)
{
    addTrack(Track(name, startValue, endValue, easing));
}

void Timeline::addTracks(std::vector<Track>&& tracks)
{
    for (Track& track : tracks)
        addTrack(std::move(track));
}

void Timeline::update(float deltaTime)
{
    // Set max delta time to duration
    deltaTime = glm::min(deltaTime * speed, duration);

    if (isStopped() || isPaused())
        return;

    if (isRewinding())
        deltaTime = -deltaTime;

    currentTime += deltaTime;

    // If looping, wrap between 0.0f and duration
    if (looping)
    {
        bool wentUnder = !rewinding ? currentTime < 0.0f : currentTime <= 0.0f;
        bool wentOver = !rewinding ? currentTime >= duration : currentTime > duration;

        currentTime += wentUnder ? duration :wentOver ? -duration : 0.0f;
        return;
    }

    if(currentTime < 0.0f)
    {
        currentTime = 0.0f;
        stop();
    }
    else if(currentTime > duration)
    {
        currentTime = duration;
        stop();
    }
}

void Timeline::seek(float time, bool paused, bool rewinding)
{
    currentTime = glm::mod(time, duration);
    this->paused = paused;
    this->rewinding = rewinding;
}

void Timeline::play()
{
    paused = false;
    stopped = false;
    rewinding = false;
}

void Timeline::pause()
{
    paused = true;
}

void Timeline::stop()
{
    stopped = true;
    paused = false;

    currentTime = rewinding ? duration : 0.0f;
}

void Timeline::rewind()
{
    play();
    rewinding = true;
}

void Timeline::resume()
{
    paused = false;
}

float Timeline::getValue(const char* name) const
{
    const Track* track = getConstTrack(name);
    return track ? track->getValue(currentTime / duration) : 0.0f;
}

float Timeline::getValuei(size_t index) const
{
    if(index >= tracksCount) return 0.0f;
    return tracks[index].getValue(currentTime / duration);
}

const char* Timeline::getName(size_t index) const
{
    if(index >= tracksCount) return "null";
    return tracks[index].getName();
}

size_t Timeline::getTracksCount() const
{
    return tracksCount;
}

Track* Timeline::getTracki(size_t index)
{
    if(index >= tracksCount) return nullptr;
    return &tracks[index];
}

Track* Timeline::getTrack(const char* name)
{
    size_t index = tracksMap[name];
    return index < tracksCount ? &tracks[index] : nullptr;
}

const Track* Timeline::getConstTracki(size_t index) const
{
    if(index >= tracksCount) return nullptr;
    return &tracks[index];
}

const Track* Timeline::getConstTrack(const char* name) const
{
    size_t index = tracksMap[name];
    return index < tracksCount ? &tracks[index] : nullptr;
}

float Timeline::getDuration() const
{
    return duration;
}

float Timeline::getCurrentTime() const
{
    return currentTime;
}

float Timeline::getSpeed() const
{
    return speed;
}

bool Timeline::isPaused() const
{
    return paused;
}

bool Timeline::isStopped() const
{
    return stopped;
}

bool Timeline::isRewinding() const
{
    return rewinding;
}

bool Timeline::isLooping() const
{
    return looping;
}

void Timeline::setLooping(bool looping)
{
    this->looping = looping;
}

void Timeline::setSpeed(float speed)
{
    this->speed = speed;
}

void Timeline::setDuration(float duration)
{
    this->duration = duration;
}

void Timeline::setCurrentTime(float currentTime)
{
    this->currentTime = currentTime;
}

#define luaGetTrack() \
    Timeline* timeline = nullptr; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(timeline, Timeline, -1);

int Timeline::lua_this(lua_State* L)
{
    if(hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "Timeline");
    lua_setmetatable(L, -2);

    luaRegisterInstance(L);

    return 1;
}

int Timeline::lua_openTimelineLib(lua_State* L)
{
    luaL_Reg timelineMethods[] =
    {
        { "addTrack", lua_addTrack },
        { "seek", lua_seek },
        { "play", lua_play },
        { "pause", lua_pause },
        { "stop", lua_stop },
        { "rewind", lua_rewind },
        { "resume", lua_resume },
        { "getValue", lua_getValue },
        { "getName", lua_getName },
        { "getTracksCount", lua_getTracksCount },
        { "getTrack", lua_getTrack },
        { "getDuration", lua_getDuration },
        { "getCurrentTime", lua_getCurrentTime },
        { "getSpeed", lua_getSpeed },
        { "isPaused", lua_isPaused },
        { "isStopped", lua_isStopped },
        { "isRewinding", lua_isRewinding },
        { "isLooping", lua_isLooping },
        { "setLooping", lua_setLooping },
        { "setSpeed", lua_setSpeed },
        { "setDuration", lua_setDuration },
        { "setCurrentTime", lua_setCurrentTime },
        { nullptr, nullptr }
    };

    Track::lua_openTrackLib(L);

    luaL_newmetatable(L, "Timeline");
    luaL_setfuncs(L, timelineMethods, 0);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_setglobal(L, "Timeline");

    LuaIndexable<Timeline>::luaRegisterType(L);

    return 1;
}

#include <iostream>

int Timeline::lua_getValue(lua_State* L)
{
    luaGetTrack();

    if(lua_isstring(L, 2))
    {
        const char* name = lua_tostring(L, 2);
        lua_pushnumber(L, timeline->getValue(name));
    }
    else if(lua_isnumber(L, 2))
    {
        size_t index = lua_tointeger(L, 2);
        lua_pushnumber(L, timeline->getValuei(index));
    }
    else
    {
        lua_pushnumber(L, 0.0f);
    }

    return 1;
}

int Timeline::lua_getName(lua_State* L)
{
    luaGetTrack();

    size_t index = lua_tointeger(L, 2);
    lua_pushstring(L, timeline->getName(index));

    return 1;
}

int Timeline::lua_getTracksCount(lua_State* L)
{
    luaGetTrack();

    lua_pushinteger(L, timeline->getTracksCount());

    return 1;
}

int Timeline::lua_getTrack(lua_State* L)
{
    luaGetTrack();

    if(lua_isstring(L, 2))
    {
        const char* name = lua_tostring(L, 2);
        Track* track = timeline->getTrack(name);
        if(track)
            track->lua_this(L);
        else
            lua_pushnil(L);
    }
    else if(lua_isnumber(L, 2))
    {
        size_t index = lua_tointeger(L, 2);
        Track* track = timeline->getTracki(index);
        if(track)
            track->lua_this(L);
        else
            lua_pushnil(L);
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

int Timeline::lua_getDuration(lua_State* L)
{
    luaGetTrack();

    lua_pushnumber(L, timeline->getDuration());

    return 1;
}

int Timeline::lua_getCurrentTime(lua_State* L)
{
    luaGetTrack();

    lua_pushnumber(L, timeline->getCurrentTime());

    return 1;
}

int Timeline::lua_getSpeed(lua_State* L)
{
    luaGetTrack();

    lua_pushnumber(L, timeline->getSpeed());

    return 1;
}

int Timeline::lua_isPaused(lua_State* L)
{
    luaGetTrack();

    lua_pushboolean(L, timeline->isPaused());

    return 1;
}

int Timeline::lua_isStopped(lua_State* L)
{
    luaGetTrack();

    lua_pushboolean(L, timeline->isStopped());

    return 1;
}

int Timeline::lua_isRewinding(lua_State* L)
{
    luaGetTrack();

    lua_pushboolean(L, timeline->isRewinding());

    return 1;
}

int Timeline::lua_isLooping(lua_State* L)
{
    luaGetTrack();

    lua_pushboolean(L, timeline->isLooping());

    return 1;
}

int Timeline::lua_setLooping(lua_State* L)
{
    luaGetTrack();

    timeline->setLooping(lua_toboolean(L, 2));

    return 0;
}

int Timeline::lua_setSpeed(lua_State* L)
{
    luaGetTrack();

    timeline->setSpeed(static_cast<float>(luaL_checknumber(L, 2)));

    return 0;
}

int Timeline::lua_setDuration(lua_State* L)
{
    luaGetTrack();

    timeline->setDuration(static_cast<float>(luaL_checknumber(L, 2)));

    return 0;
}

int Timeline::lua_setCurrentTime(lua_State* L)
{
    luaGetTrack();

    timeline->setCurrentTime(static_cast<float>(luaL_checknumber(L, 2)));

    return 0;
}

int Timeline::lua_play(lua_State* L)
{
    luaGetTrack();

    timeline->play();

    return 0;
}

int Timeline::lua_pause(lua_State* L)
{
    luaGetTrack();

    timeline->pause();

    return 0;
}

int Timeline::lua_stop(lua_State* L)
{
    luaGetTrack();

    timeline->stop();

    return 0;
}

int Timeline::lua_rewind(lua_State* L)
{
    luaGetTrack();

    timeline->rewind();

    return 0;
}

int Timeline::lua_resume(lua_State* L)
{
    luaGetTrack();

    timeline->resume();

    return 0;
}

int Timeline::lua_addTrack(lua_State* L)
{
    luaGetTrack();

    const char* name = luaL_checkstring(L, 2);

    if(lua_istable(L, 3))
    {
        std::vector<Keyframe> keyframes;
        lua_pushnil(L);
        while(lua_next(L, 3) != 0)
        {
            float time = static_cast<float>(luaL_checknumber(L, -2));
            float value = static_cast<float>(luaL_checknumber(L, -1));
            keyframes.push_back({time, value});
            lua_pop(L, 1);
        }
        timeline->addTrack(name, std::move(keyframes));

        return 0;
    }
    
    if(lua_gettop(L) == 3)
        timeline->addTrack(name, static_cast<float>(luaL_checknumber(L, 3)));
    else if(lua_gettop(L) == 4)
        timeline->addTrack(name, static_cast<float>(luaL_checknumber(L, 3)), static_cast<float>(luaL_checknumber(L, 4)));
    else if(lua_gettop(L) == 5)
    {
        float startValue = static_cast<float>(luaL_checknumber(L, 3));
        float endValue = static_cast<float>(luaL_checknumber(L, 4));
        Easing::Function function = Easing::getEasingFunction(luaL_checkstring(L, 5));
        timeline->addTrack(name, startValue, endValue, function);
    }

    return 0;
}

int Timeline::lua_seek(lua_State* L)
{
    luaGetTrack();

    int top = lua_gettop(L);

    float time = static_cast<float>(luaL_checknumber(L, 2));
    if(top == 3)
    {
        bool paused = lua_toboolean(L, 3);
        timeline->seek(time, paused);
    }
    else if(top == 4)
    {
        bool paused = lua_toboolean(L, 3);
        bool rewinding = lua_toboolean(L, 4);
        timeline->seek(time, paused, rewinding);
    }
    else
        timeline->seek(time);

    return 0;
}