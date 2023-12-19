#pragma once

#include <MeshBuilder.hpp>
#include <Framebuffer.hpp>
#include <Logger.hpp>
#include <memory>
#include <LuaInclude.hpp>

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

        int lua_this(lua_State* L);

        static int lua_openCompositorLib(lua_State* L);
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

        static int lua_getCompositor(lua_State* L, Compositor** compositor);
        static int lua_setGamma(lua_State* L);
        static int lua_setExposure(lua_State* L);
        static int lua_getGamma(lua_State* L);
        static int lua_getExposure(lua_State* L);
    };
}