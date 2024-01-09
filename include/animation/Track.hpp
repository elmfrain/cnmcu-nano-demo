#pragma once

#include <animation/Easing.hpp>
#include <array>
#include <vector>
#include <cstddef>

namespace em
{
    struct Keyframe
    {
        float time = 0.0f; // normalized time
        float value = 0.0f;
        Easing::Function easing = Easing::linear;
    };

    class Track
    {
    public:
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
    };
}