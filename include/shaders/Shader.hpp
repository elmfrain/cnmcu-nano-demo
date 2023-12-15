#pragma once

#include <GLInclude.hpp>
#include <glm/glm.hpp>
#include <Logger.hpp>

namespace em
{
    class Shader
    {
    public:
        Shader();

        virtual bool init();
        void use();
        void destroy();

        void setProjectionMatrix(const glm::mat4& projectionMatrix);
        void setModelViewMatrix(const glm::mat4& modelViewMatrix);
        void setColor(const glm::vec4& color);
        void setEnabledTexture(int index); // -1 to disable textures
        void setVertexColorEnabled(bool enabled);

        GLuint getProgram() const;
        const glm::mat4& getProjectionMatrix() const;
        const glm::mat4& getModelViewMatrix() const;
        const glm::vec4& getColor() const;
        int getEnabledTexture() const; // returns -1 if textures are disabled
        bool isVertexColorEnabled() const;

        const char* getName() const;
    private:
        GLuint m_projectionMatrixUniformLoc;
        GLuint m_modelViewMatrixUniformLoc;
        GLuint m_colorUniformLoc;
        GLuint m_texturesUniformLoc;
        GLuint m_textureCountUniformLoc;
        GLuint m_enabledTextureUniformLoc;
        GLuint m_vertexColorEnabledUniformLoc;

        glm::mat4 m_projectionMatrix;
        glm::mat4 m_modelViewMatrix;
        glm::vec4 m_color;
        GLuint m_textures[32];
        GLuint m_textureCount;
        int m_enabledTexture;
        bool m_vertexColorEnabled;

        bool m_hasWarnedAboutNotBeingInitialized;

        static const char* basicVertexShaderSource;
        static const char* basicFragmentShaderSource;

        static GLuint getMaxTextureUnits();
    protected:
        const char* m_name;
        static Logger m_logger;
        GLuint m_program;

        GLuint generateProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
        void getDefaultUniformLocations();
        virtual void updateUniforms();
    };
}