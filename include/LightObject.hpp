#pragma once

#include <SceneObject.hpp>

namespace em
{
    class LightObject : public SceneObject
    {
    public:
        LightObject(const std::string& name = "LightObject");
        virtual ~LightObject();

        virtual void update(float dt) override;
        virtual void draw(Shader& shader) override;

        void setColor(const glm::vec3& color);
        glm::vec3 getColor() const;

        void setIntensity(float intensity);
        float getIntensity() const;
    private:
        glm::vec3 color;
        float intensity;
    };
}