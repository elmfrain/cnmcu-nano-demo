#pragma once 

#include <animation/Track.hpp>

#include <unordered_map>
#include <vector>
#include <string>
#include <LuaInclude.hpp>

namespace em
{
    class Timeline : private LuaIndexable<Timeline>
    {
    public:
        Timeline();
        Timeline(std::vector<Track>&& tracks);

        Timeline(const Timeline& other) = delete;
        Timeline operator=(const Timeline& other) = delete;

        Timeline(Timeline&& other);
        Timeline& operator=(Timeline&& other);

        void addTrack(Track&& track);
        void addTrack(const char* name, float value);
        void addTrack(const char* name, std::vector<Keyframe>&& keyframes);
        void addTrack(const char* name, float startValue, float endValue, Easing::Function easing = Easing::linear);

        void addTracks(std::vector<Track>&& tracks);

        void update(float deltaTime);

        void seek(float time, bool paused = false, bool rewinding = false);

        void play();
        void pause();
        void stop();
        void rewind();
        void resume();

        float getValue(const char* name) const;
        float getValuei(size_t index) const;
        const char* getName(size_t index) const;
        size_t getTracksCount() const;
        Track* getTracki(size_t index);
        Track* getTrack(const char* name);
        const Track* getConstTracki(size_t index) const;
        const Track* getConstTrack(const char* name) const;

        float getDuration() const;
        float getCurrentTime() const;
        float getSpeed() const;
        bool isPaused() const;
        bool isStopped() const;
        bool isRewinding() const;
        bool isLooping() const;

        void setLooping(bool looping);
        void setSpeed(float speed);
        void setDuration(float duration);
        void setCurrentTime(float currentTime);

        int lua_this(lua_State* L);
        static int lua_openTimelineLib(lua_State* L);
    private:
        std::array<Track, 8> tracksSmallList;
        std::vector<Track> tracksLargeList;
        Track* tracks = tracksSmallList.data();
        size_t tracksCount = 0;
        mutable std::unordered_map<std::string, size_t> tracksMap;

        float duration = 1.0f;
        float currentTime = 0.0f;
        float speed = 1.0f;
        bool paused = false;
        bool stopped = true;
        bool rewinding = false;
        bool looping = false;

        static int lua_getValue(lua_State* L);
        static int lua_getName(lua_State* L);
        static int lua_getTracksCount(lua_State* L);
        static int lua_getTrack(lua_State* L);
        static int lua_getDuration(lua_State* L);
        static int lua_getCurrentTime(lua_State* L);
        static int lua_getSpeed(lua_State* L);
        static int lua_isPaused(lua_State* L);
        static int lua_isStopped(lua_State* L);
        static int lua_isRewinding(lua_State* L);
        static int lua_isLooping(lua_State* L);

        static int lua_setLooping(lua_State* L);
        static int lua_setSpeed(lua_State* L);
        static int lua_setDuration(lua_State* L);
        static int lua_setCurrentTime(lua_State* L);

        static int lua_play(lua_State* L);
        static int lua_pause(lua_State* L);
        static int lua_stop(lua_State* L);
        static int lua_rewind(lua_State* L);
        static int lua_resume(lua_State* L);

        static int lua_addTrack(lua_State* L);
        static int lua_seek(lua_State* L);
    };
}