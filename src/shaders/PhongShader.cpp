#include "shaders/PhongShader.hpp"

using namespace em;

const char* PhongShader::m_vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 inPos;\n"
    "layout(location = 1) in vec2 inUV;\n"
    "layout(location = 2) in vec3 inNormal;\n"
    "layout(location = 3) in vec4 inColor;\n"
    "out vec2 uv;\n"
    "out vec4 color;\n"
    "out vec3 normal;\n"
    "out vec3 fragPos;\n"
    "uniform mat4 u_projectionMatrix;\n"
    "uniform mat4 u_viewMatrix;\n"
    "uniform mat4 u_modelViewMatrix;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_projectionMatrix * u_modelViewMatrix * vec4(inPos, 1);\n"
    "    uv = inUV;\n"
    "    color = inColor;\n"
    "    normal = inNormal;\n"
    "    fragPos = vec3(u_modelViewMatrix * vec4(inPos, 1.0));\n"
    "}\n";

const char* PhongShader::m_fragmentShaderSource =
    "#version 330 core\n"
    "in vec2 uv;\n"
    "in vec3 normal;\n"
    "in vec4 color;\n"
    "in vec3 fragPos;\n"
    "out vec4 outColor;\n"
    "uniform sampler2D u_textures[32];\n"
    "uniform int u_textureCount;\n"
    "uniform int u_enabledTexture;\n"
    "uniform vec4 u_color;\n"
    "uniform bool u_vertexColorEnabled;\n"
    "uniform vec3 u_cameraPos;\n"
    "vec3 lightPos = vec3(0.0, 2.0, 0.0);"
    "void main()\n"
    "{\n"
    "    vec3 lightDir = normalize(lightPos - fragPos);\n"
    "    vec3 viewDir = normalize(u_cameraPos - fragPos);\n"
    "    vec3 reflectDir = reflect(-lightDir, normal);\n"
    "    float distance = length(lightPos - fragPos);\n"
    "    float intensity = 4.0 / (distance * distance);\n"
    "    float diff = max(dot(normal, lightDir), 0.0);\n"
    "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);\n"
    "    outColor = vec4(vec3(diff * intensity + spec), 1.0);\n"
    "    if(u_enabledTexture >= 0) outColor *= texture(u_textures[u_enabledTexture], uv);\n"
    "}\n";

PhongShader::PhongShader() : Shader()
{
    m_name = "PhongShader";
}

bool PhongShader::init()
{
    if(!(m_program = generateProgram(m_vertexShaderSource, m_fragmentShaderSource)))
        return false;

    getDefaultUniformLocations();
    m_cameraPosUniformLocation = glGetUniformLocation(m_program, "u_cameraPos");

    return true;
}

void PhongShader::setCameraPos(const glm::vec3& pos)
{
    m_cameraPos = glm::vec3(pos.x, pos.y, -pos.z);
}

void PhongShader::updateUniforms()
{
    Shader::updateUniforms();

    glUniform3fv(m_cameraPosUniformLocation, 1, &m_cameraPos[0]);
}