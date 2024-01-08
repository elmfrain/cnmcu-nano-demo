#include "animation/Track.hpp"

#include <algorithm>
#include <cstring>

using namespace em;

Track::Track(const char* name)
{
    memset(this->name, 0, 32);
    strncpy(this->name, name, 31);
}

Track::Track(const char* name, float value)
{
    memset(this->name, 0, 32);
    strncpy(this->name, name, 31);

    addKeyframe(0.0f, value);
    addKeyframe(1.0f, value);
}

Track::Track(const char* name, const std::vector<Keyframe>& keyframes)
{
    memset(this->name, 0, 32);
    strncpy(this->name, name, 31);

    addKeyframes(keyframes);

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

void Track::addKeyframe(const Keyframe& keyframe)
{
    if (keyframesCount == 8)
    {
        keyframesLargeList = std::vector<Keyframe>(keyframesSmallList.begin(), keyframesSmallList.end());
        keyframes = keyframesLargeList.data();
    }

    for(size_t i = 0; i < keyframesCount; i++)
        if (keyframes[i].time == keyframe.time)
        {
            keyframes[i] = keyframe;
            return;
        }

    sortKeyframes();
}

void Track::addKeyframe(float time, float value, Easing::Function easing)
{
    addKeyframe({ time, value, easing });
}

void Track::addKeyframes(const std::vector<Keyframe>& keyframes)
{
    for (const auto& keyframe : keyframes)
        addKeyframe(keyframe);
}

float Track::getValue(float time) const
{
    if (keyframesCount == 0)
        return 0.0f;

    if (keyframesCount == 1)
        return keyframes[0].value;

    findKeyframes(time);

    float t = (time - currentKeyframe->time) / (nextKeyframe->time - currentKeyframe->time);
    return currentKeyframe->value + (nextKeyframe->value - currentKeyframe->value) * currentKeyframe->easing(t);
}

const char* Track::getName() const
{
    return name;
}

void Track::sortKeyframes()
{
    std::sort(keyframes, keyframes + keyframesCount, [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
}

void Track::findKeyframes(float time) const
{
    if (time < keyframes[0].time)
    {
        currentKeyframe = &keyframes[0];
        nextKeyframe = &keyframes[1];
        return;
    }

    if (time > keyframes[keyframesCount - 1].time)
    {
        currentKeyframe = &keyframes[keyframesCount - 2];
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