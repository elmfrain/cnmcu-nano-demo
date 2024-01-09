#include <gtest/gtest.h>

#include <animation/Track.hpp>

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
    Track track("test");
    ASSERT_STREQ(track.getName(), "test");
    ASSERT_EQ(track.getKeyframesCount(), 2);

    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), 0.0f) << "t = " << t;

    track = Track("test", 1.0f);
    ASSERT_EQ(track.getKeyframesCount(), 2);

    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), 1.0f) << "t = " << t;

    track = Track("test", 1.0f, 2.0f);

    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), 1.0f + t) << "t = " << t;

    track = Track("test", 1.0f, 2.0f, Easing::quadInOut);

    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), Easing::quadInOut(t) + 1.0f) <<  "t = " << t;

    track = Track("test", {
        { 0.0f, 0.0f },
        { 0.5f, 1.0f },
        { 1.0f, 0.0f }
    });

    ASSERT_EQ(track.getKeyframesCount(), 3);

    track.addKeyframe(0.5f, 1.0f);

    ASSERT_EQ(track.getKeyframesCount(), 3);

    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), t < 0.5f ? t * 2.0f : 2.0f - t * 2.0f) << "t = " << t;

    track.addKeyframe(0.25f, 2.0f);

    for(float t = 0.0f; t <= 1.0f; t += 0.125f)
        ASSERT_FLOAT_EQ(track.getValue(t), t < 0.25f ? t * 8.0f : t < 0.5f ? 2.0f - (t - 0.25f) * 4.0f : 1.0f - (t - 0.5f) * 2.0f) << "t = " << t;
}