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
        { "linear", Easing::linear },
        { "instant", Easing::instant },
        { "sineIn", Easing::sineIn },
        { "sineOut", Easing::sineOut },
        { "sineInOut", Easing::sineInOut },
        { "sineOutIn", Easing::sineOutIn },
        { "quadIn", Easing::quadIn },
        { "quadOut", Easing::quadOut },
        { "quadInOut", Easing::quadInOut },
        { "quadOutIn", Easing::quadOutIn },
        { "cubicIn", Easing::cubicIn },
        { "cubicOut", Easing::cubicOut },
        { "cubicInOut", Easing::cubicInOut },
        { "cubicOutIn", Easing::cubicOutIn },
        { "quartIn", Easing::quartIn },
        { "quartOut", Easing::quartOut },
        { "quartInOut", Easing::quartInOut },
        { "quartOutIn", Easing::quartOutIn },
        { "quintIn", Easing::quintIn },
        { "quintOut", Easing::quintOut },
        { "quintInOut", Easing::quintInOut },
        { "quintOutIn", Easing::quintOutIn },
        { "expoIn", Easing::expoIn },
        { "expoOut", Easing::expoOut },
        { "expoInOut", Easing::expoInOut },
        { "expoOutIn", Easing::expoOutIn },
        { "circIn", Easing::circIn },
        { "circOut", Easing::circOut },
        { "circInOut", Easing::circInOut },
        { "circOutIn", Easing::circOutIn },
        { "backIn", Easing::backIn },
        { "backOut", Easing::backOut },
        { "backInOut", Easing::backInOut },
        { "backOutIn", Easing::backOutIn },
        { "elasticIn", Easing::elasticIn },
        { "elasticOut", Easing::elasticOut },
        { "elasticInOut", Easing::elasticInOut },
        { "elasticOutIn", Easing::elasticOutIn },
        { "bounceIn", Easing::bounceIn },
        { "bounceOut", Easing::bounceOut },
        { "bounceInOut", Easing::bounceInOut },
        { "bounceOutIn", Easing::bounceOutIn },
        { "smoothStep", Easing::smoothStep }
    };

    auto it = easingFunctions.find(name);

    if (it != easingFunctions.end())
        return it->second;
            
    return nullptr;
}