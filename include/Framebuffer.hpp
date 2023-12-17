#pragma once 

#include <GLInclude.hpp>
#include <Logger.hpp>

namespace em
{
    class Framebuffer
    {
    public:
        Framebuffer();
        ~Framebuffer();

        void init(int width, int height);
        void bind();
        void unbind();
        void destroy();

        int getWidth() const;
        int getHeight() const;

        void resize(int width, int height);

        GLuint getTexture() const;
        GLuint getHandle() const;

        void clearColors();
        void clearDepth();
        void clearStencil();
        void clear();
    private:
        int m_width;
        int m_height;

        GLuint m_framebuffer;
        GLuint m_colorTexture;
        GLuint m_depthStencilRenderbuffer;

        static Logger logger;
    };
} // namespace em
