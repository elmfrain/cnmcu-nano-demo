#include <shaders/Shader.hpp>

using namespace em;

const char* Shader::m_vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 inPos;\n"
    "layout(location = 1) in vec2 inUV;\n"
    "layout(location = 2) in vec4 inColor;\n"
    "out vec2 uv;\n"
    "out vec4 color;\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 modelViewMatrix;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projectionMatrix * modelViewMatrix * vec4(inPos, 1);\n"
    "    uv = inUV;\n"
    "    color = inColor;\n"
    "}\n";

const char* Shader::m_fragmentShaderSource =
    "#version 330 core\n"
    "in vec2 uv;\n"
    "in vec4 color;\n"
    "out vec4 outColor;\n"
    "uniform sampler2D textures[32];\n"
    "uniform int textureCount;\n"
    "uniform int enabledTexture;\n"
    "void main()\n"
    "{\n"
    "    if (enabledTexture >= 0 && enabledTexture < textureCount)\n"
    "        outColor = texture(textures[enabledTexture], uv) * color;\n"
    "    else\n"
    "        outColor = color;\n"
    "}\n";

Logger Shader::m_logger("Shaders");

Shader::Shader()
    : m_program(0)
    , m_projectionMatrixUniformLoc(0)
    , m_modelViewMatrixUniformLoc(0)
    , m_colorUniformLoc(0)
    , m_texturesUniformLoc(0)
    , m_textureCountUniformLoc(0)
    , m_enabledTextureUniformLoc(0)
    , m_projectionMatrix(1)
    , m_modelViewMatrix(1)
    , m_color(1)
    , m_textureCount(0)
    , m_enabledTexture(-1)
    , m_name("BasicShader")
    , m_hasWarnedAboutNotBeingInitialized(false)
{
}

bool Shader::init()
{
    if(m_program != 0)
    {
        m_logger.warnf("Shader %s is already initialized", getName());
        return true;
    }

    m_logger.infof("Initializing shader %s", getName());

    m_textureCount = getMaxTextureUnits();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &m_vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        m_logger.errorf("Vertex shader compilation failed for %s: %s", getName(), infoLog);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &m_fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        m_logger.errorf("Fragment shader compilation failed for %s: %s", getName(), infoLog);
        return false;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_program, 512, NULL, infoLog);
        m_logger.errorf("Shader program linking failed for %s: %s", getName(), infoLog);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_projectionMatrixUniformLoc = glGetUniformLocation(m_program, "projectionMatrix");
    m_modelViewMatrixUniformLoc = glGetUniformLocation(m_program, "modelViewMatrix");
    m_colorUniformLoc = glGetUniformLocation(m_program, "color");
    m_texturesUniformLoc = glGetUniformLocation(m_program, "textures");
    m_textureCountUniformLoc = glGetUniformLocation(m_program, "textureCount");
    m_enabledTextureUniformLoc = glGetUniformLocation(m_program, "enabledTexture");

    return true;
}

void Shader::use()
{
    if(m_program == 0)
    {
        if(!m_hasWarnedAboutNotBeingInitialized)
        {
            m_logger.warnf("Shader %s is not initialized or has failed initializing", getName());
            m_hasWarnedAboutNotBeingInitialized = true;
        }
        return;
    }

    updateUniforms();

    glUseProgram(m_program);
}

void Shader::destroy()
{
    if(m_program == 0)
    {
        m_logger.warnf("Shader %s is not initialized or has failed initializing", getName());
        return;
    }

    m_logger.infof("Destroying shader %s", getName());

    glDeleteProgram(m_program);
    m_program = 0;
}

void Shader::setProjectionMatrix(const glm::mat4& projectionMatrix)
{
    m_projectionMatrix = projectionMatrix;
}

void Shader::setModelViewMatrix(const glm::mat4& modelViewMatrix)
{
    m_modelViewMatrix = modelViewMatrix;
}

void Shader::setColor(const glm::vec4& color)
{
    m_color = color;
}

void Shader::setEnabledTexture(int index)
{
    m_enabledTexture = index;
}

GLuint Shader::getProgram() const
{
    return m_program;
}

const glm::mat4& Shader::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

const glm::mat4& Shader::getModelViewMatrix() const
{
    return m_modelViewMatrix;
}

const glm::vec4& Shader::getColor() const
{
    return m_color;
}

int Shader::getEnabledTexture() const
{
    return m_enabledTexture;
}

const char* Shader::getName() const
{
    return m_name;
}

GLuint Shader::getMaxTextureUnits()
{
    static bool hasAdvisedMaxTextureUnits = false;

    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    if(!hasAdvisedMaxTextureUnits)
    {
        m_logger.infof("Maximum texture units: %d", maxTextureUnits);
        hasAdvisedMaxTextureUnits = true;
    }
    return maxTextureUnits;
}

void Shader::updateUniforms()
{
    glUniformMatrix4fv(m_projectionMatrixUniformLoc, 1, GL_FALSE, &m_projectionMatrix[0][0]);
    glUniformMatrix4fv(m_modelViewMatrixUniformLoc, 1, GL_FALSE, &m_modelViewMatrix[0][0]);
    glUniform4fv(m_colorUniformLoc, 1, &m_color[0]);
    glUniform1iv(m_texturesUniformLoc, 32, (GLint*)m_textures);
    glUniform1i(m_textureCountUniformLoc, m_textureCount);
    glUniform1i(m_enabledTextureUniformLoc, m_enabledTexture);
}