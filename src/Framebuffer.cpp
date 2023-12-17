#include <Framebuffer.hpp>

using namespace em;

Logger Framebuffer::logger("Framebuffer");

Framebuffer::Framebuffer()
    : m_framebuffer(0)
    , m_colorTexture(0)
    , m_depthStencilRenderbuffer(0)
{
}

Framebuffer::~Framebuffer()
{
}

void Framebuffer::init(int width, int height)
{
    m_width = std::max(width, 1);
    m_height = std::max(height, 1);

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    glGenRenderbuffers(1, &m_depthStencilRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRenderbuffer);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger.errorf("Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::destroy()
{
    glDeleteFramebuffers(1, &m_framebuffer);
    glDeleteTextures(1, &m_colorTexture);
    glDeleteRenderbuffers(1, &m_depthStencilRenderbuffer);
}

int Framebuffer::getWidth() const
{
    return m_width;
}

int Framebuffer::getHeight() const
{
    return m_height;
}

void Framebuffer::resize(int width, int height)
{
    m_width = std::max(width, 1);
    m_height = std::max(height, 1);

    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);

    clear();
}

GLuint Framebuffer::getTexture() const
{
    return m_colorTexture;
}

GLuint Framebuffer::getHandle() const
{
    return m_framebuffer;
}

void Framebuffer::clearColors()
{
    bind();
    glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::clearDepth()
{
    bind();
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::clearStencil()
{
    bind();
    glClear(GL_STENCIL_BUFFER_BIT);
}

void Framebuffer::clear()
{
    bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
