#pragma once

#include <SceneObject.hpp>

namespace em
{
    class LightObject : public SceneObject
    {
    public:
        LightObject(const std::string& name = "LightObject");
        virtual ~LightObject();

        virtual void draw(Shader& shader) override;

        void setColor(const glm::vec3& color);
        glm::vec3 getColor() const;

        void setIntensity(float intensity);
        float getIntensity() const;

        int lua_this(lua_State* L) override;
        static int lua_openLightObjectLib(lua_State* L);
    private:
        glm::vec3 color;
        float intensity;

        static int lua_getColor(lua_State* L);
        static int lua_getIntensity(lua_State* L);

        static int lua_setColor(lua_State* L);
        static int lua_setIntensity(lua_State* L);
    protected:
        virtual void update(float dt) override;
    };
}