#pragma once 

#include "Shader.hpp"

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