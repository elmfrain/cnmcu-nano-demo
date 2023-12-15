#include <shaders/Shader.hpp>

using namespace em;

const char* Shader::basicVertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 inPos;\n"
    "layout(location = 1) in vec2 inUV;\n"
    "layout(location = 2) in vec4 inColor;\n"
    "out vec2 uv;\n"
    "out vec4 color;\n"
    "uniform mat4 u_projectionMatrix;\n"
    "uniform mat4 u_modelViewMatrix;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_projectionMatrix * u_modelViewMatrix * vec4(inPos, 1);\n"
    "    uv = inUV;\n"
    "    color = inColor;\n"
    "}\n";

const char* Shader::basicFragmentShaderSource =
    "#version 330 core\n"
    "in vec2 uv;\n"
    "in vec4 color;\n"
    "out vec4 outColor;\n"
    "uniform sampler2D u_textures[32];\n"
    "uniform int u_textureCount;\n"
    "uniform int u_enabledTexture;\n"
    "uniform vec4 u_color;\n"
    "uniform bool u_vertexColorEnabled;\n"
    "void main()\n"
    "{\n"
    "    vec4 vertexColor = u_vertexColorEnabled ? color : vec4(1.0f);\n"
    "    if (u_enabledTexture >= 0)\n"
    "        outColor = texture(u_textures[u_enabledTexture], uv) * vertexColor * u_color;\n"
    "    else\n"
    "        outColor = vertexColor * u_color;\n"
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
    , m_vertexColorEnabled(false)
    , m_name("BasicShader")
    , m_hasWarnedAboutNotBeingInitialized(false)
{
  for(int i = 0; i < sizeof(m_textures) / sizeof(GLuint); i++)
    m_textures[i] = i;
}

bool Shader::init()
{
    if((m_program = generateProgram(basicVertexShaderSource, basicFragmentShaderSource)) == 0)
        return false;

    getDefaultUniformLocations();

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

    updateDefaultUniforms();

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

void Shader::setVertexColorEnabled(bool enabled)
{
    m_vertexColorEnabled = enabled;
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

bool Shader::isVertexColorEnabled() const
{
    return m_vertexColorEnabled;
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

GLuint Shader::generateProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        m_logger.errorf("Vertex shader compilation failed for %s: %s", getName(), infoLog);
        return 0;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        m_logger.errorf("Fragment shader compilation failed for %s: %s", getName(), infoLog);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        m_logger.errorf("Shader program linking failed for %s: %s", getName(), infoLog);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Shader::getDefaultUniformLocations()
{
    m_projectionMatrixUniformLoc = glGetUniformLocation(m_program, "u_projectionMatrix");
    m_modelViewMatrixUniformLoc = glGetUniformLocation(m_program, "u_modelViewMatrix");
    m_colorUniformLoc = glGetUniformLocation(m_program, "u_color");
    m_texturesUniformLoc = glGetUniformLocation(m_program, "u_textures");
    m_textureCountUniformLoc = glGetUniformLocation(m_program, "u_textureCount");
    m_enabledTextureUniformLoc = glGetUniformLocation(m_program, "u_enabledTexture");
    m_vertexColorEnabledUniformLoc = glGetUniformLocation(m_program, "u_vertexColorEnabled");
}

void Shader::updateDefaultUniforms()
{
    glUniformMatrix4fv(m_projectionMatrixUniformLoc, 1, GL_FALSE, &m_projectionMatrix[0][0]);
    glUniformMatrix4fv(m_modelViewMatrixUniformLoc, 1, GL_FALSE, &m_modelViewMatrix[0][0]);
    glUniform4fv(m_colorUniformLoc, 1, &m_color[0]);
    glUniform1iv(m_texturesUniformLoc, 32, (GLint*)m_textures);
    glUniform1i(m_textureCountUniformLoc, m_textureCount);
    glUniform1i(m_enabledTextureUniformLoc, m_enabledTexture);
    glUniform1i(m_vertexColorEnabledUniformLoc, m_vertexColorEnabled);
}