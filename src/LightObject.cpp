#include <LightObject.hpp>

using namespace em;

LightObject::LightObject(const std::string& name)
    : SceneObject(LIGHT, name)
    , color(1.0f, 1.0f, 1.0f)
    , intensity(1.0f)
{
}

LightObject::~LightObject()
{
}

void LightObject::update(float dt)
{
}

void LightObject::draw(Shader& shader)
{
}

void LightObject::setColor(const glm::vec3& color)
{
    this->color = color;
}

glm::vec3 LightObject::getColor() const
{
    return color;
}

void LightObject::setIntensity(float intensity)
{
    this->intensity = intensity;
}

float LightObject::getIntensity() const
{
    return intensity;
}