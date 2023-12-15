#pragma once 

#include "Shader.hpp"

namespace em
{
    class PhongShader : public Shader
    {
    public:
        PhongShader();

        bool init() override;

        void setCameraPos(const glm::vec3& pos);
    private:
        GLuint m_cameraPosUniformLocation;

        glm::vec3 m_cameraPos;

        static const char* m_vertexShaderSource;
        static const char* m_fragmentShaderSource;
    protected:
        void updateUniforms() override;
    };
}