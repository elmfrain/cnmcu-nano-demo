
#pragma once

#include <animation/Easing.hpp>
#include <array>
#include <vector>
#include <cstddef>

#include <LuaInclude.hpp>

namespace em
{
    struct Keyframe
    {
        float time = 0.0f; // normalized time
        float value = 0.0f;
        Easing::Function easing = Easing::linear;
        LuaIndexable<Keyframe> luaIndexable;

        int lua_this(lua_State* L);
        static int lua_openKeyframeLib(lua_State* L);
    private:
        static int lua_getTime(lua_State* L);
        static int lua_getValue(lua_State* L);
        static int lua_getEasing(lua_State* L);

        static int lua_setTime(lua_State* L);
        static int lua_setValue(lua_State* L);
        static int lua_setEasing(lua_State* L);
    };

    class Track : private LuaIndexable<Track>
    {
    public:
        Track();
        Track(const char* name);
        Track(const char* name, float value);
        Track(const char* name, const std::vector<Keyframe>& keyframes);
        Track(const char* name, float startValue, float endValue, Easing::Function easing = Easing::linear);

        void addKeyframe(const Keyframe& keyframe);
        void addKeyframe(float time, float value, Easing::Function easing = Easing::linear);
        void addKeyframes(const std::vector<Keyframe>& keyframes);

        float getValue(float time) const;
        const char* getName() const;
        size_t getKeyframesCount() const;

        int lua_this(lua_State* L);
        static int lua_openTrackLib(lua_State* L);
    private:

        char name[32];

        std::array<Keyframe, 8> keyframesSmallList;
        std::vector<Keyframe> keyframesLargeList;

        Keyframe* keyframes = keyframesSmallList.data();
        size_t keyframesCount = 0;

        mutable Keyframe* currentKeyframe = nullptr;
        mutable Keyframe* nextKeyframe = nullptr;

        void sortKeyframes();
        void findKeyframes(float time) const;

        static int lua_getName(lua_State* L);
        static int lua_getKeyframesCount(lua_State* L);
        static int lua_getValue(lua_State* L);

        static int lua_addKeyframe(lua_State* L);
    };
}