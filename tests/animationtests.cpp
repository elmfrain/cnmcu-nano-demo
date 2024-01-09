#include <gtest/gtest.h>

#include <animation/Track.hpp>
#include <animation/Timeline.hpp>

#include <glm/glm.hpp>

using namespace em;

TEST(Animation, Easing)
{
    ASSERT_FLOAT_EQ(Easing::linear(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::linear(0.5f), 0.5f);
    ASSERT_FLOAT_EQ(Easing::linear(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quadIn(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quadIn(0.5f), 0.25f);
    ASSERT_FLOAT_EQ(Easing::quadIn(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quadOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quadOut(0.5f), 0.75f);
    ASSERT_FLOAT_EQ(Easing::quadOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quadInOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quadInOut(0.5f), 0.5f);
    ASSERT_FLOAT_EQ(Easing::quadInOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::cubicIn(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::cubicIn(0.5f), 0.125f);
    ASSERT_FLOAT_EQ(Easing::cubicIn(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::cubicOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::cubicOut(0.5f), 0.875f);
    ASSERT_FLOAT_EQ(Easing::cubicOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::cubicInOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::cubicInOut(0.5f), 0.5f);
    ASSERT_FLOAT_EQ(Easing::cubicInOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quartIn(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quartIn(0.5f), 0.0625f);
    ASSERT_FLOAT_EQ(Easing::quartIn(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quartOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quartOut(0.5f), 0.9375f);
    ASSERT_FLOAT_EQ(Easing::quartOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quartInOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quartInOut(0.5f), 0.5f);
    ASSERT_FLOAT_EQ(Easing::quartInOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quintIn(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quintIn(0.5f), 0.03125f);
    ASSERT_FLOAT_EQ(Easing::quintIn(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quintOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quintOut(0.5f), 0.96875f);
    ASSERT_FLOAT_EQ(Easing::quintOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::quintInOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::quintInOut(0.5f), 0.5f);
    ASSERT_FLOAT_EQ(Easing::quintInOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::sineIn(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::sineIn(0.5f), 0.2928932188134524f);
    ASSERT_FLOAT_EQ(Easing::sineIn(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::sineOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::sineOut(0.5f), 0.7071067811865476f);
    ASSERT_FLOAT_EQ(Easing::sineOut(1.0f), 1.0f);

    ASSERT_FLOAT_EQ(Easing::sineInOut(0.0f), 0.0f);
    ASSERT_FLOAT_EQ(Easing::sineInOut(0.5f), 0.5f);
    ASSERT_FLOAT_EQ(Easing::sineInOut(1.0f), 1.0f);
}

TEST(Animation, Track)
{
    // Check if Track is initialized with name and (2) default keyframes
    Track track("test");
    ASSERT_STREQ(track.getName(), "test");
    ASSERT_EQ(track.getKeyframesCount(), 2);

    // Check if empty track returns 0.0f throughout the whole time
    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), 0.0f) << "t = " << t;


    // Check if Track is initialized with name and (2) default keyframes
    track = Track("test", 1.0f);
    ASSERT_EQ(track.getKeyframesCount(), 2);

    // Check if track with one keyframe returns 1.0f throughout the whole time
    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), 1.0f) << "t = " << t;

    track = Track("test", 1.0f, 2.0f);

    // Check if track with two keyframes returns 1.0f at t = 0.0f and 2.0f at t = 1.0f
    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), 1.0f + t) << "t = " << t;

    track = Track("test", 1.0f, 2.0f, Easing::quadInOut);

    // Check if track with two keyframes returns 1.0f at t = 0.0f and 2.0f at t = 1.0f with quadInOut easing
    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), Easing::quadInOut(t) + 1.0f) <<  "t = " << t;

    track = Track("test", {
        { 0.0f, 0.0f },
        { 0.5f, 1.0f },
        { 1.0f, 0.0f }
    });

    // Check if adding a keyframe with the same time as an existing one does not change the keyframes count
    ASSERT_EQ(track.getKeyframesCount(), 3);
    track.addKeyframe(0.5f, 1.0f);
    ASSERT_EQ(track.getKeyframesCount(), 3);

    // Test if track interpolates correctly between keyframes
    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), t < 0.5f ? t * 2.0f : 2.0f - t * 2.0f) << "t = " << t;

    track.addKeyframe(0.25f, 2.0f);

    // Test again but with a new keyframe at t = 0.25f
    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), t < 0.25f ? t * 8.0f : t < 0.5f ? 2.0f - (t - 0.25f) * 4.0f : 1.0f - (t - 0.5f) * 2.0f) << "t = " << t;
}

TEST(Animation, Timeline)
{
    Timeline timeline;

    // Be sure that timeline is initialized with no tracks
    ASSERT_EQ(timeline.getTracksCount(), 0);

    // Check if adding and indexing tracks works
    timeline.addTrack("test", 1.0f);
    ASSERT_EQ(timeline.getTracksCount(), 1);
    ASSERT_EQ(timeline.getTracki(0)->getKeyframesCount(), 2);
    ASSERT_EQ(timeline.getTrack("test")->getKeyframesCount(), 2);

    // Check if adding a track with the same name as an existing one does not change the tracks count
    timeline.addTrack("test", 2.0f);
    ASSERT_EQ(timeline.getTracksCount(), 1);

    timeline = Timeline();

    Track track("test", {
        { 0.0f, 0.0f },
        { 1.0f, 1.0f }
    });

    timeline.addTrack(track);

    timeline.play();

    for(float t = 0.0f; t <= 1.25f; t += 0.125f)
    {
        ASSERT_FLOAT_EQ(timeline.getValue("test"), t <= 1.0f ? t : 0.0f) << "t = " << t;
        timeline.update(0.125f);
    }

    ASSERT_TRUE(timeline.isStopped());

    timeline.rewind();

    ASSERT_TRUE(timeline.isRewinding());
    timeline.update(0.125f);
    ASSERT_TRUE(timeline.isStopped());

    timeline.rewind();

    for(float t = 0.0f; t <= 1.25f; t += 0.125f)
    {
        ASSERT_FLOAT_EQ(timeline.getValue("test"), t <= 1.0f ? 1.0f - t : 1.0f) << "t = " << t;
        timeline.update(0.125f);
    }

    ASSERT_TRUE(timeline.isStopped());

    timeline.play();
    timeline.update(0.125f);
    timeline.setDuration(2.0f);

    timeline.play();

    for(float t = 0.0f; t <= 2.25f; t += 0.125f)
    {
        ASSERT_FLOAT_EQ(timeline.getValue("test"), t <= 2.0f ? t / 2.0f : 0.0f) << "t = " << t;
        timeline.update(0.125f);
    }

    timeline.play();

    for(float t = 0.0f; t <= 1.5f; t += 0.125f)
    {
        if(t == 1.0f) timeline.stop();
        timeline.update(0.125f);
    }

    ASSERT_TRUE(timeline.isStopped());
    ASSERT_FLOAT_EQ(timeline.getValue("test"), 0.0f);

    timeline.play();

    for(float t = 0.0f; t <= 1.5f; t += 0.125f)
    {
        if(t == 1.0f) timeline.pause();
        timeline.update(0.125f);
    }

    ASSERT_TRUE(timeline.isPaused());
    ASSERT_FLOAT_EQ(timeline.getValue("test"), 0.5f);

    timeline.stop();
    timeline.setSpeed(2.0f);
    timeline.update(0.125f);

    timeline.play();

    for(float t = 0.0f; t <= 1.25f; t += 0.125f)
    {
        ASSERT_FLOAT_EQ(timeline.getValue("test"), t <= 1.0f ? t : 0.0f) << "t = " << t;
        timeline.update(0.125f);
    }

    ASSERT_TRUE(timeline.isStopped());
    timeline.setSpeed(1.0f);
    timeline.setDuration(1.0f);
    timeline.setLooping(true);

    timeline.play();

    for(float t = 0.0f; t <= 5.0f; t += 0.125f)
    {
        ASSERT_FLOAT_EQ(timeline.getValue("test"), glm::mod(t, 1.0f)) << "t = " << t;
        timeline.update(0.125f);
    }

    timeline.stop();
    ASSERT_FLOAT_EQ(timeline.getValue("test"), 0.0f);

    timeline.rewind();
    timeline.stop();
    timeline.rewind();

    for(float t = 0.0f; t <= 5.0f; t += 0.125f)
    {
        ASSERT_FLOAT_EQ(timeline.getValue("test"), 1.0f - glm::mod(t, 1.0f)) << "t = " << t;
        timeline.update(0.125f);
    }

    timeline.stop();
    ASSERT_FLOAT_EQ(timeline.getValue("test"), 1.0f);

    timeline.setLooping(false);
    timeline.setCurrentTime(0.0f);
    timeline.play();

    bool paused = false;
    float x = 0.0f;

    for(float t = 0.0f; t <= 1.5f; t += 0.125f)
    {
        if(t == 0.5f)
        {
            paused = true;
            timeline.pause();
        }

        if(t == 1.0f)
        {
            paused = false;
            timeline.resume();
        }

        ASSERT_FLOAT_EQ(timeline.getValue("test"), x) << "t = " << t;
        if(!paused) x += 0.125f;
        timeline.update(0.125f);
    }
}