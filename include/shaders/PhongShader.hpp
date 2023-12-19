#pragma once 

#include "Shader.hpp"
#include <LuaInclude.hpp>

#define MAX_LIGHTS 8

namespace em
{
    struct PhongMaterial
    {
        glm::vec3 ambient = glm::vec3(0.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(0.5f);
        float shininess = 32.0f;

        void roughnessToShininess(float roughness)
        {
            shininess = 1.0f / (roughness * roughness);
        }

        int lua_this(lua_State* L);
        static int lua_openPhongMaterialLib(lua_State* L);

    private:
        static int lua_getPhongMaterial(lua_State* L, PhongMaterial** material);

        static int lua_getAmbient(lua_State* L);
        static int lua_getDiffuse(lua_State* L);
        static int lua_getSpecular(lua_State* L);
        static int lua_getShininess(lua_State* L);
        static int lua_getRoughness(lua_State* L);

        static int lua_setAmbient(lua_State* L);
        static int lua_setDiffuse(lua_State* L);
        static int lua_setSpecular(lua_State* L);
        static int lua_setShininess(lua_State* L);
        static int lua_setRoughness(lua_State* L);
    public:
        static int fromLua(lua_State* L, int index, PhongMaterial& material)
        {
            if(luaHasKeyValue(L, "ambient", index))
            {
                luaPushValueFromKey("ambient", index);
                luaGetVec3(material.ambient, -1);
            }

            if(luaHasKeyValue(L, "diffuse", index))
            {
                luaPushValueFromKey("diffuse", index);
                int type = lua_rawlen(L, -1);
                luaGetVec3(material.diffuse, -1);
            }

            if(luaHasKeyValue(L, "specular", index))
            {
                luaPushValueFromKey("specular", index);
                luaGetVec3(material.specular, -1);
            }

            if(luaHasKeyValue(L, "shininess", index))
            {
                luaPushValueFromKey("shininess", index);
                luaGet(material.shininess, float, number, -1);
            }

            if(luaHasKeyValue(L, "roughness", index))
            {
                luaPushValueFromKey("roughness", index);
                float roughness;
                luaGet(roughness, float, number, -1);
                material.roughnessToShininess(roughness);
            }

            return 0;
        }
    };

    struct PhongLight
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 emission = glm::vec3(0.0f);

        void setLight(glm::vec3 color, float intensity)
        {
            emission = color * intensity;
        }
    };

    class PhongShader : public Shader
    {
    public:
        PhongShader();

        bool init() override;

        void setCameraPos(const glm::vec3& pos);

        void setMaterial(const PhongMaterial& material);
        PhongMaterial getMaterial() const;

        PhongLight& getLight(int index);
        void setLightCount(int count);
    private:
        GLuint m_cameraPosUniformLocation;
        GLuint m_materialAmbientUniformLocation;
        GLuint m_materialDiffuseUniformLocation;
        GLuint m_materialSpecularUniformLocation;
        GLuint m_materialShininessUniformLocation;
        GLuint m_lightsPositionUniformLocation[MAX_LIGHTS];
        GLuint m_lightsEmissionUniformLocation[MAX_LIGHTS];
        GLuint m_lightCountUniformLocation;

        glm::vec3 m_cameraPos;
        PhongMaterial m_material;
        PhongLight m_lights[MAX_LIGHTS];
        int m_lightCount;

        static const char* m_vertexShaderSource;
        static const char* m_fragmentShaderSource;
    protected:
        void updateUniforms() override;
    };
}