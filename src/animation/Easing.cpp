#include "animation/Easing.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using namespace em;

float Easing::linear(float x)
{
    return x;
}

float Easing::instant(float x)
{
    return 1.0f;
}

float Easing::sineIn(float x)
{
    return 1.0f - glm::cos(x * glm::half_pi<float>());
}

float Easing::sineOut(float x)
{
    return glm::sin(x * glm::half_pi<float>());
}

float Easing::sineInOut(float x)
{
    return 0.5f * (1.0f - glm::cos(x * glm::pi<float>()));
}

float Easing::sineOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f * glm::sin(x * glm::pi<float>());
    else
        return 0.5f + 0.5f * (1.0f - glm::cos((x - 0.5f) * glm::pi<float>()));
}

float Easing::quadIn(float x)
{
    return x * x;
}

float Easing::quadOut(float x)
{
    return 1.0f - (1.0f - x) * (1.0f - x);
}

float Easing::quadInOut(float x)
{
    if (x < 0.5f)
        return 2.0f * x * x;
    else
        return 1.0f - 2.0f * (1.0f - x) * (1.0f - x);
}

float Easing::quadOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f - 0.5f * (1.0f - x) * (1.0f - x);
    else
        return 0.5f + 0.5f * x * x;
}

float Easing::cubicIn(float x)
{
    return x * x * x;
}

float Easing::cubicOut(float x)
{
    return 1.0f - (1.0f - x) * (1.0f - x) * (1.0f - x);
}

float Easing::cubicInOut(float x)
{
    if (x < 0.5f)
        return 4.0f * x * x * x;
    else
        return 1.0f - 4.0f * (1.0f - x) * (1.0f - x) * (1.0f - x);
}

float Easing::cubicOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f - 0.5f * (1.0f - x) * (1.0f - x) * (1.0f - x);
    else
        return 0.5f + 0.5f * x * x * x;
}

float Easing::quartIn(float x)
{
    return x * x * x * x;
}

float Easing::quartOut(float x)
{
    return 1.0f - (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x);
}

float Easing::quartInOut(float x)
{
    if (x < 0.5f)
        return 8.0f * x * x * x * x;
    else
        return 1.0f - 8.0f * (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x);
}

float Easing::quartOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f - 0.5f * (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x);
    else
        return 0.5f + 0.5f * x * x * x * x;
}

float Easing::quintIn(float x)
{
    return x * x * x * x * x;
}

float Easing::quintOut(float x)
{
    return 1.0f - (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x);
}

float Easing::quintInOut(float x)
{
    if (x < 0.5f)
        return 16.0f * x * x * x * x * x;
    else
        return 1.0f - 16.0f * (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x);
}

float Easing::quintOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f - 0.5f * (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x) * (1.0f - x);
    else
        return 0.5f + 0.5f * x * x * x * x * x;
}

float Easing::expoIn(float x)
{
    return glm::pow(2.0f, 10.0f * (x - 1.0f));
}

float Easing::expoOut(float x)
{
    return 1.0f - glm::pow(2.0f, -10.0f * x);
}

float Easing::expoInOut(float x)
{
    if (x < 0.5f)
        return 0.5f * glm::pow(2.0f, 10.0f * (2.0f * x - 1.0f));
    else
        return 0.5f * (2.0f - glm::pow(2.0f, -10.0f * (2.0f * x - 1.0f)));
}

float Easing::expoOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f - 0.5f * glm::pow(2.0f, -20.0f * x);
    else
        return 0.5f + 0.5f * glm::pow(2.0f, 20.0f * (x - 1.0f));
}

float Easing::circIn(float x)
{
    return 1.0f - glm::sqrt(1.0f - x * x);
}

float Easing::circOut(float x)
{
    return glm::sqrt(1.0f - (1.0f - x) * (1.0f - x));
}

float Easing::circInOut(float x)
{
    if (x < 0.5f)
        return 0.5f * (1.0f - glm::sqrt(1.0f - 4.0f * x * x));
    else
        return 0.5f * (1.0f + glm::sqrt(-3.0f + 8.0f * x - 4.0f * x * x));
}

float Easing::circOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f * glm::sqrt(1.0f - 4.0f * (x - 0.5f) * (x - 0.5f));
    else
        return 0.5f + 0.5f * glm::sqrt(-3.0f + 8.0f * (x - 0.5f) - 4.0f * (x - 0.5f) * (x - 0.5f));
}

float Easing::backIn(float x)
{
    return x * x * (2.70158f * x - 1.70158f);
}

float Easing::backOut(float x)
{
    return 1.0f - (1.0f - x) * (1.0f - x) * (-2.70158f * (1.0f - x) - 1.70158f);
}

float Easing::backInOut(float x)
{
    if (x < 0.5f)
        return 2.0f * x * x * (7.189819f * x - 2.5949095f);
    else
        return 1.0f - 2.0f * (1.0f - x) * (1.0f - x) * (-7.189819f * (1.0f - x) - 2.5949095f);
}

float Easing::backOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f - 0.5f * (1.0f - x) * (1.0f - x) * (-2.70158f * (1.0f - x) - 1.70158f);
    else
        return 0.5f + 0.5f * x * x * (2.70158f * x - 1.70158f);
}

float Easing::elasticIn(float x)
{
    return glm::sin(13.0f * glm::half_pi<float>() * x) * glm::pow(2.0f, 10.0f * (x - 1.0f));
}

float Easing::elasticOut(float x)
{
    return glm::sin(-13.0f * glm::half_pi<float>() * (x + 1.0f)) * glm::pow(2.0f, -10.0f * x) + 1.0f;
}

float Easing::elasticInOut(float x)
{
    if (x < 0.5f)
        return 0.5f * glm::sin(13.0f * glm::half_pi<float>() * (2.0f * x)) * glm::pow(2.0f, 10.0f * (2.0f * x - 1.0f));
    else
        return 0.5f * (glm::sin(-13.0f * glm::half_pi<float>() * (2.0f * x - 1.0f)) * glm::pow(2.0f, -10.0f * (2.0f * x - 1.0f)) + 2.0f);
}

float Easing::elasticOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f * (glm::sin(-13.0f * glm::half_pi<float>() * (2.0f * x + 1.0f)) * glm::pow(2.0f, -10.0f * (2.0f * x + 1.0f)) + 2.0f);
    else
        return 0.5f * glm::sin(13.0f * glm::half_pi<float>() * (2.0f * x - 1.0f)) * glm::pow(2.0f, 10.0f * (2.0f * x - 1.0f));
}

float Easing::bounceIn(float x)
{
    return 1.0f - bounceOut(1.0f - x);
}

float Easing::bounceOut(float x)
{
    if (x < 4.0f / 11.0f)
        return (121.0f * x * x) / 16.0f;
    else if (x < 8.0f / 11.0f)
        return (363.0f / 40.0f * x * x) - (99.0f / 10.0f * x) + 17.0f / 5.0f;
    else if (x < 9.0f / 10.0f)
        return (4356.0f / 361.0f * x * x) - (35442.0f / 1805.0f * x) + 16061.0f / 1805.0f;
    else
        return (54.0f / 5.0f * x * x) - (513.0f / 25.0f * x) + 268.0f / 25.0f;
}

float Easing::bounceInOut(float x)
{
    if (x < 0.5f)
        return 0.5f * bounceIn(x * 2.0f);
    else
        return 0.5f * bounceOut(x * 2.0f - 1.0f) + 0.5f;
}

float Easing::bounceOutIn(float x)
{
    if (x < 0.5f)
        return 0.5f * bounceOut(x * 2.0f);
    else
        return 0.5f * bounceIn(x * 2.0f - 1.0f) + 0.5f;
}

float Easing::smoothStep(float x)
{
    return x * x * (3.0f - 2.0f * x);
}

#include <unordered_map>
#include <string>

Easing::Function Easing::getEasingFunction(const char* name)
{
    static std::unordered_map<std::string, Easing::Function> easingFunctions =
    {
        { "LINEAR", Easing::linear },
        { "INSTANT", Easing::instant },
        { "SINE_IN", Easing::sineIn },
        { "SINE_OUT", Easing::sineOut },
        { "SINE_INOUT", Easing::sineInOut },
        { "SINE_OUTIN", Easing::sineOutIn },
        { "QUAD_IN", Easing::quadIn },
        { "QUAD_OUT", Easing::quadOut },
        { "QUAD_INOUT", Easing::quadInOut },
        { "QUAD_OUTIN", Easing::quadOutIn },
        { "CUBIC_IN", Easing::cubicIn },
        { "CUBIC_OUT", Easing::cubicOut },
        { "CUBIC_INOUT", Easing::cubicInOut },
        { "CUBIC_OUTIN", Easing::cubicOutIn },
        { "QUART_IN", Easing::quartIn },
        { "QUART_OUT", Easing::quartOut },
        { "QUART_INOUT", Easing::quartInOut },
        { "QUART_OUTIN", Easing::quartOutIn },
        { "QUINT_IN", Easing::quintIn },
        { "QUINT_OUT", Easing::quintOut },
        { "QUINT_INOUT", Easing::quintInOut },
        { "QUINT_OUTIN", Easing::quintOutIn },
        { "EXPO_IN", Easing::expoIn },
        { "EXPO_OUT", Easing::expoOut },
        { "EXPO_INOUT", Easing::expoInOut },
        { "EXPO_OUTIN", Easing::expoOutIn },
        { "CIRC_IN", Easing::circIn },
        { "CIRC_OUT", Easing::circOut },
        { "CIRC_INOUT", Easing::circInOut },
        { "CIRC_OUTIN", Easing::circOutIn },
        { "BACK_IN", Easing::backIn },
        { "BACK_OUT", Easing::backOut },
        { "BACK_INOUT", Easing::backInOut },
        { "BACK_OUTIN", Easing::backOutIn },
        { "ELASTIC_IN", Easing::elasticIn },
        { "ELASTIC_OUT", Easing::elasticOut },
        { "ELASTIC_INOUT", Easing::elasticInOut },
        { "ELASTIC_OUTIN", Easing::elasticOutIn },
        { "BOUNCE_IN", Easing::bounceIn },
        { "BOUNCE_OUT", Easing::bounceOut },
        { "BOUNCE_INOUT", Easing::bounceInOut },
        { "BOUNCE_OUTIN", Easing::bounceOutIn },
        { "SMOOTHSTEP", Easing::smoothStep }
    };

    auto it = easingFunctions.find(name);

    if (it != easingFunctions.end())
        return it->second;
            
    return Easing::linear;
}

const char* Easing::getEasingFunctionName(Easing::Function easingFunction)
{
    static std::unordered_map<Easing::Function, const char*> easingFunctionNames =
    {
        { Easing::linear, "LINEAR" },
        { Easing::instant, "INSTANT" },
        { Easing::sineIn, "SINE_IN" },
        { Easing::sineOut, "SINE_OUT" },
        { Easing::sineInOut, "SINE_INOUT" },
        { Easing::sineOutIn, "SINE_OUTIN" },
        { Easing::quadIn, "QUAD_IN" },
        { Easing::quadOut, "QUAD_OUT" },
        { Easing::quadInOut, "QUAD_INOUT" },
        { Easing::quadOutIn, "QUAD_OUTIN" },
        { Easing::cubicIn, "CUBIC_IN" },
        { Easing::cubicOut, "CUBIC_OUT" },
        { Easing::cubicInOut, "CUBIC_INOUT" },
        { Easing::cubicOutIn, "CUBIC_OUTIN" },
        { Easing::quartIn, "QUART_IN" },
        { Easing::quartOut, "QUART_OUT" },
        { Easing::quartInOut, "QUART_INOUT" },
        { Easing::quartOutIn, "QUART_OUTIN" },
        { Easing::quintIn, "QUINT_IN" },
        { Easing::quintOut, "QUINT_OUT" },
        { Easing::quintInOut, "QUINT_INOUT" },
        { Easing::quintOutIn, "QUINT_OUTIN" },
        { Easing::expoIn, "EXPO_IN" },
        { Easing::expoOut, "EXPO_OUT" },
        { Easing::expoInOut, "EXPO_INOUT" },
        { Easing::expoOutIn, "EXPO_OUTIN" },
        { Easing::circIn, "CIRC_IN" },
        { Easing::circOut, "CIRC_OUT" },
        { Easing::circInOut, "CIRC_INOUT" },
        { Easing::circOutIn, "CIRC_OUTIN" },
        { Easing::backIn, "BACK_IN" },
        { Easing::backOut, "BACK_OUT" },
        { Easing::backInOut, "BACK_INOUT" },
        { Easing::backOutIn, "BACK_OUTIN" },
        { Easing::elasticIn, "ELASTIC_IN" },
        { Easing::elasticOut, "ELASTIC_OUT" },
        { Easing::elasticInOut, "ELASTIC_INOUT" },
        { Easing::elasticOutIn, "ELASTIC_OUTIN" },
        { Easing::bounceIn, "BOUNCE_IN" },
        { Easing::bounceOut, "BOUNCE_OUT" },
        { Easing::bounceInOut, "BOUNCE_INOUT" },
        { Easing::bounceOutIn, "BOUNCE_OUTIN" },
        { Easing::smoothStep, "SMOOTHSTEP" }
    };

    auto it = easingFunctionNames.find(easingFunction);

    if (it != easingFunctionNames.end())
        return it->second;

    return "UNKNOWN";
}