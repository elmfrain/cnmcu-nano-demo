#pragma once

#include <MeshBuilder.hpp>
#include <Framebuffer.hpp>
#include <Logger.hpp>
#include <memory>

namespace em
{
    class Compositor
    {
    public:
        Compositor();
        ~Compositor();

        void init();
        void blit(Framebuffer& framebuffer);
        void destroy();

        void setGamma(float gamma);
        void setExposure(float exposure);

        float getGamma() const;
        float getExposure() const;
    private:
        std::unique_ptr<MeshBuilder> m_meshBuilder;

        GLuint m_program;
        GLuint m_textureUniformLoc;
        GLuint m_gammaUniformLoc;
        GLuint m_exposureUniformLoc;
        GLuint m_widthUniformLoc;
        GLuint m_heightUniformLoc;

        float m_gamma;
        float m_exposure;

        bool m_hasWarnedAboutNotBeingInitialized;

        GLuint generateShaderProgram();
        void updateUniforms(Framebuffer& framebuffer);

        static const char* vertexShaderSource;
        static const char* fragmentShaderSource;
        static Logger m_logger;
    };
}