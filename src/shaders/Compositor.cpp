#include <shaders/Compositor.hpp>

using namespace em;

const char* Compositor::vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 inPos;\n"
    "layout(location = 1) in vec2 inUV;\n"
    "out vec2 uv;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(inPos, 1);\n"
    "    uv = inUV;\n"
    "}\n";

const char* Compositor::fragmentShaderSource =
    "#version 330 core\n"
    "in vec2 uv;\n"
    "out vec4 outColor;\n"
    "uniform sampler2D u_texture;\n"
    "uniform float u_gamma;\n"
    "uniform float u_exposure;\n"
    "uniform float u_width;\n"
    "uniform float u_height;\n"
    "void main()\n"
    "{\n"
    "    vec4 color = texture(u_texture, uv);\n"
    "    color.rgb = color.rgb * u_exposure;\n"
    "    color.rgb = pow(color.rgb, vec3(u_gamma));\n"
    "    outColor = color;\n"
    "}\n";

Logger Compositor::m_logger("Compositor");

Compositor::Compositor() :
    m_program(0),
    m_gamma(2.2f),
    m_exposure(1.0f)
{
    VertexFormat vertexFormat;
    vertexFormat.size = 2;
    vertexFormat[0].data = EMVF_ATTRB_USAGE_POS
                         | EMVF_ATTRB_TYPE_FLOAT
                         | EMVF_ATTRB_SIZE(3)
                         | EMVF_ATTRB_NORMALIZED_FALSE;
    vertexFormat[1].data = EMVF_ATTRB_USAGE_UV
                         | EMVF_ATTRB_TYPE_FLOAT
                         | EMVF_ATTRB_SIZE(2)
                         | EMVF_ATTRB_NORMALIZED_FALSE;
    
    m_meshBuilder = std::make_unique<MeshBuilder>(vertexFormat);
}

Compositor::~Compositor()
{
}

void Compositor::init()
{
    m_program = generateShaderProgram();
    m_textureUniformLoc = glGetUniformLocation(m_program, "u_texture");
    m_gammaUniformLoc = glGetUniformLocation(m_program, "u_gamma");
    m_exposureUniformLoc = glGetUniformLocation(m_program, "u_exposure");
    m_widthUniformLoc = glGetUniformLocation(m_program, "u_width");
    m_heightUniformLoc = glGetUniformLocation(m_program, "u_height");

    m_meshBuilder->
    index(6, 0, 1, 2, 2, 3, 0)
    .vertex(nullptr, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f)
    .vertex(nullptr,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f)
    .vertex(nullptr,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f)
    .vertex(nullptr, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f);
}

void Compositor::blit(Framebuffer& framebuffer)
{
    if(m_program == 0)
    {
        if(!m_hasWarnedAboutNotBeingInitialized)
        {
            m_logger.warnf("Compositor is not initialized!");
            m_hasWarnedAboutNotBeingInitialized = true;
        }
        return;
    }

    glUseProgram(m_program);
    updateUniforms(framebuffer);

    m_meshBuilder->drawElements(GL_TRIANGLES);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Compositor::destroy()
{
    m_meshBuilder.reset();

    glDeleteProgram(m_program);
}


void Compositor::setGamma(float gamma)
{
    m_gamma = gamma;
}

void Compositor::setExposure(float exposure)
{
    m_exposure = exposure;
}

float Compositor::getGamma() const
{
    return m_gamma;
}

float Compositor::getExposure() const
{
    return m_exposure;
}

GLuint Compositor::generateShaderProgram()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        m_logger.errorf("Failed to compile vertex shader: %s", infoLog);
        return 0;
    }

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        m_logger.errorf("Failed to compile fragment shader: %s", infoLog);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glBindAttribLocation(program, 0, "inPos");
    glBindAttribLocation(program, 1, "inUV");

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        m_logger.errorf("Failed to link shader program: %s", infoLog);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Compositor::updateUniforms(Framebuffer& framebuffer)
{
    glUniform1f(m_gammaUniformLoc, m_gamma);
    glUniform1f(m_exposureUniformLoc, m_exposure);
    glUniform1f(m_widthUniformLoc, framebuffer.getWidth());
    glUniform1f(m_heightUniformLoc, framebuffer.getHeight());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.getTexture());
    glUniform1i(m_textureUniformLoc, 0);
}