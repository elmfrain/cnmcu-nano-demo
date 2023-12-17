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
    "struct Material\n"
    "{\n"
    "    vec3 ambient;\n"
    "    vec3 diffuse;\n"
    "    vec3 specular;\n"
    "    float shininess;\n"
    "};\n"
    "uniform Material u_material;\n"
    "struct Light\n"
    "{\n"
    "    vec3 position;\n"
    "    vec3 emission;\n"
    "};\n"
    "uniform Light u_lights[8];\n"
    "uniform int u_lightCount;\n"
    "vec3 calcLight(Light light, vec3 viewDir)\n"
    "{\n"
    "    float distance = length(light.position - fragPos);\n"
    "    vec3 lightDir = normalize(light.position - fragPos);\n"
    "    float diff = max(dot(normal, lightDir), 0.0);\n"
    "    vec3 reflectDir = reflect(-lightDir, normal);\n"
    "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);\n"
    "    vec3 ambient = light.emission * u_material.ambient;\n"
    "    vec3 diffuse = light.emission * diff * u_material.diffuse / (distance * distance + 1.0);\n"
    "    vec3 specular = light.emission * spec * u_material.specular;\n"
    "    return (ambient + diffuse + specular);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    outColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "    vec3 viewDir = normalize(u_cameraPos - fragPos);\n"
    "    for(int i = 0; i < u_lightCount; i++)\n"
    "    {\n"
    "        outColor += vec4(calcLight(u_lights[i], viewDir), 0.0);\n"
    "    }\n"
    "    outColor *= u_color;\n"
    "    if(u_vertexColorEnabled) outColor *= color;\n"
    "    if(u_enabledTexture >= 0) outColor *= texture(u_textures[u_enabledTexture], uv);\n"
    "}\n";

PhongShader::PhongShader() : Shader()
    , m_cameraPos(0.0f)
    , m_material()
    , m_lightCount(0)
{
    m_name = "PhongShader";
}

bool PhongShader::init()
{
    if(!(m_program = generateProgram(m_vertexShaderSource, m_fragmentShaderSource)))
        return false;

    getDefaultUniformLocations();
    m_cameraPosUniformLocation = glGetUniformLocation(m_program, "u_cameraPos");
    m_materialAmbientUniformLocation = glGetUniformLocation(m_program, "u_material.ambient");
    m_materialDiffuseUniformLocation = glGetUniformLocation(m_program, "u_material.diffuse");
    m_materialSpecularUniformLocation = glGetUniformLocation(m_program, "u_material.specular");
    m_materialShininessUniformLocation = glGetUniformLocation(m_program, "u_material.shininess");
    m_lightCountUniformLocation = glGetUniformLocation(m_program, "u_lightCount");

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "u_lights[%d].position", i);
        m_lightsPositionUniformLocation[i] = glGetUniformLocation(m_program, buffer);
        snprintf(buffer, sizeof(buffer), "u_lights[%d].emission", i);
        m_lightsEmissionUniformLocation[i] = glGetUniformLocation(m_program, buffer);
    }

    return true;
}

void PhongShader::setCameraPos(const glm::vec3& pos)
{
    m_cameraPos = glm::vec3(pos.x, pos.y, -pos.z);
}

void PhongShader::setMaterial(const PhongMaterial& material)
{
    m_material = material;
}

PhongMaterial PhongShader::getMaterial() const
{
    return m_material;
}

PhongLight& PhongShader::getLight(int index)
{
    return m_lights[index];
}

void PhongShader::setLightCount(int count)
{
    m_lightCount = count;
}

void PhongShader::updateUniforms()
{
    Shader::updateUniforms();

    glUniform3fv(m_cameraPosUniformLocation, 1, &m_cameraPos[0]);
    glUniform3fv(m_materialAmbientUniformLocation, 1, &m_material.ambient[0]);
    glUniform3fv(m_materialDiffuseUniformLocation, 1, &m_material.diffuse[0]);
    glUniform3fv(m_materialSpecularUniformLocation, 1, &m_material.specular[0]);
    glUniform1f(m_materialShininessUniformLocation, m_material.shininess);
    glUniform1i(m_lightCountUniformLocation, m_lightCount);

    for(int i = 0; i < m_lightCount; i++)
    {
        glUniform3fv(m_lightsPositionUniformLocation[i], 1, &m_lights[i].position[0]);
        glUniform3fv(m_lightsEmissionUniformLocation[i], 1, &m_lights[i].emission[0]);
    }
}