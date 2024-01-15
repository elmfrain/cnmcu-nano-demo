#pragma once

namespace em
{
    class Easing
    {
    public:
        typedef float (*Function)(float);

        static float linear(float x);
        static float instant(float x);

        static float sineIn(float x);
        static float sineOut(float x);
        static float sineInOut(float x);
        static float sineOutIn(float x);

        static float quadIn(float x);
        static float quadOut(float x);
        static float quadInOut(float x);
        static float quadOutIn(float x);

        static float cubicIn(float x);
        static float cubicOut(float x);
        static float cubicInOut(float x);
        static float cubicOutIn(float x);

        static float quartIn(float x);
        static float quartOut(float x);
        static float quartInOut(float x);
        static float quartOutIn(float x);

        static float quintIn(float x);
        static float quintOut(float x);
        static float quintInOut(float x);
        static float quintOutIn(float x);

        static float expoIn(float x);
        static float expoOut(float x);
        static float expoInOut(float x);
        static float expoOutIn(float x);

        static float circIn(float x);
        static float circOut(float x);
        static float circInOut(float x);
        static float circOutIn(float x);

        static float backIn(float x);
        static float backOut(float x);
        static float backInOut(float x);
        static float backOutIn(float x);

        static float elasticIn(float x);
        static float elasticOut(float x);
        static float elasticInOut(float x);
        static float elasticOutIn(float x);

        static float bounceIn(float x);
        static float bounceOut(float x);
        static float bounceInOut(float x);
        static float bounceOutIn(float x);

        static float smoothStep(float x);

        static Function getEasingFunction(const char* name);
        static const char* getEasingFunctionName(Function easingFunction);
    };
}