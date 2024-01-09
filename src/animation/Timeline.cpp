#include "animation/Timeline.hpp"

#include <glm/glm.hpp>

using namespace em;

Timeline::Timeline()
{
}

Timeline::Timeline(const std::vector<Track>& tracks)
{
    addTracks(tracks);
}

void Timeline::addTrack(const Track& track)
{
    // Replace track if it already exists
    if (tracksMap.count(track.getName()) > 0)
    {
        tracks[tracksMap[track.getName()]] = track;
        return;
    }

    if (tracksCount < 8)
    {
        tracks[tracksCount] = track;
        tracksMap[track.getName()] = tracksCount;
        tracksCount++;
    }
    else
    {
        tracksLargeList.push_back(track);
        tracks = tracksLargeList.data();
        tracksMap[track.getName()] = tracksCount;
        tracksCount++;
    }
}

void Timeline::addTrack(const char* name, float value)
{
    addTrack(Track(name, value));
}

void Timeline::addTrack(const char* name, const std::vector<Keyframe>& keyframes)
{
    addTrack(Track(name, keyframes));
}

void Timeline::addTrack(const char* name, float startValue, float endValue, Easing::Function easing)
{
    addTrack(Track(name, startValue, endValue, easing));
}

void Timeline::addTracks(const std::vector<Track>& tracks)
{
    for (const Track& track : tracks)
        addTrack(track);
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

float Timeline::getValue(size_t index) const
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