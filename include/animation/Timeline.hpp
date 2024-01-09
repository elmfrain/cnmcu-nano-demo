#pragma once 

#include <animation/Track.hpp>

#include <unordered_map>
#include <vector>

namespace em
{
    class Timeline
    {
    public:
        Timeline();
        Timeline(const std::vector<Track>& tracks);

        void addTrack(const Track& track);
        void addTrack(const char* name, float value);
        void addTrack(const char* name, const std::vector<Keyframe>& keyframes);
        void addTrack(const char* name, float startValue, float endValue, Easing::Function easing = Easing::linear);

        void addTracks(const std::vector<Track>& tracks);

        void update(float deltaTime);

        void seek(float time, bool paused = false, bool rewinding = false);

        void play();
        void pause();
        void stop();
        void rewind();
        void resume();

        float getValue(const char* name) const;
        float getValue(size_t index) const;
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

    private:
        std::array<Track, 8> tracksSmallList;
        std::vector<Track> tracksLargeList;
        mutable std::unordered_map<const char*, size_t> tracksMap;
        Track* tracks = tracksSmallList.data();
        size_t tracksCount = 0;

        float duration = 1.0f;
        float currentTime = 0.0f;
        float speed = 1.0f;
        bool paused = false;
        bool stopped = true;
        bool rewinding = false;
        bool looping = false;
    };
}