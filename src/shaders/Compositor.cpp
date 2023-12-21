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
    "vec3 acesTonemap(vec3 x)\n"
    "{\n"
    "    float a = 2.51f;\n"
    "    float b = 0.03f;\n"
    "    float c = 2.43f;\n"
    "    float d = 0.59f;\n"
    "    float e = 0.14f;\n"
    "    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    vec4 color = texture(u_texture, uv);\n"
    "    color.rgb = color.rgb * u_exposure;\n"
    "    float grayscale = dot(color.rgb, vec3(0.21, 0.72, 0.07));\n" // Convert to grayscale (color desaturation)
    "    color.rgb += grayscale * grayscale;\n" // Mix with grayscale (color desaturation from luminance)
    "    color.rgb = acesTonemap(color.rgb);\n" // Apply ACES tonemapping
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

int Compositor::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "Compositor");
    lua_setmetatable(L, -2);

    return 1;
}

int Compositor::lua_openCompositorLib(lua_State* L)
{
    static const luaL_Reg luaCompositorMethods[] =
    {
        {"setGamma", lua_setGamma},
        {"setExposure", lua_setExposure},
        {"getGamma", lua_getGamma},
        {"getExposure", lua_getExposure},
        {nullptr, nullptr}
    };

    luaL_newmetatable(L, "Compositor");
    luaL_setfuncs(L, luaCompositorMethods, 0);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_setglobal(L, "Compositor");

    return 0;
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
    glUniform1f(m_gammaUniformLoc, 1.0f / m_gamma);
    glUniform1f(m_exposureUniformLoc, m_exposure);
    glUniform1f(m_widthUniformLoc, framebuffer.getWidth());
    glUniform1f(m_heightUniformLoc, framebuffer.getHeight());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.getTexture());
    glUniform1i(m_textureUniformLoc, 0);
}

#define luaGetCompositor() \
    Compositor* compositor; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(compositor, Compositor, -1);

int Compositor::lua_setGamma(lua_State* L)
{
    luaGetCompositor();

    float gamma;
    luaGet(gamma, float, number, 2);

    compositor->setGamma(gamma);

    return 0;
}

int Compositor::lua_setExposure(lua_State* L)
{
    luaGetCompositor();

    float exposure;
    luaGet(exposure, float, number, 2);

    compositor->setExposure(exposure);

    return 0;
}

int Compositor::lua_getGamma(lua_State* L)
{
    luaGetCompositor();

    lua_pushnumber(L, compositor->m_gamma);

    return 1;
}

int Compositor::lua_getExposure(lua_State* L)
{
    luaGetCompositor();

    lua_pushnumber(L, compositor->m_exposure);

    return 1;
}