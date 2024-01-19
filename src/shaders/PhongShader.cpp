#include "shaders/PhongShader.hpp"

using namespace em;

#define luaGetPhongMaterial() \
    PhongMaterial* material; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(material, PhongMaterial, -1);

int PhongMaterial::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ptr");

    luaL_newmetatable(L, "PhongMaterial");
    lua_setmetatable(L, -2);

    return 1;
}

int PhongMaterial::lua_openPhongMaterialLib(lua_State* L)
{
    static const luaL_Reg luaPhongMaterialMethods[] =
    {
        {"getAmbient", lua_getAmbient},
        {"getDiffuse", lua_getDiffuse},
        {"getSpecular", lua_getSpecular},
        {"getShininess", lua_getShininess},
        {"getRoughness", lua_getRoughness},
        {"setAmbient", lua_setAmbient},
        {"setDiffuse", lua_setDiffuse},
        {"setSpecular", lua_setSpecular},
        {"setShininess", lua_setShininess},
        {"setRoughness", lua_setRoughness},
        {nullptr, nullptr}
    };

    luaL_newmetatable(L, "PhongMaterial");
    luaL_setfuncs(L, luaPhongMaterialMethods, 0);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_setglobal(L, "PhongMaterial");

    return 0;
}

int PhongMaterial::lua_getAmbient(lua_State* L)
{
    luaGetPhongMaterial();

    luaPushVec3(material->ambient);

    return 1;
}

int PhongMaterial::lua_getDiffuse(lua_State* L)
{
    luaGetPhongMaterial();

    luaPushVec3(material->diffuse);

    return 1;
}

int PhongMaterial::lua_getSpecular(lua_State* L)
{
    luaGetPhongMaterial();

    luaPushVec3(material->specular);

    return 1;
}

int PhongMaterial::lua_getShininess(lua_State* L)
{
    luaGetPhongMaterial();

    lua_pushnumber(L, material->shininess);

    return 1;
}

int PhongMaterial::lua_getRoughness(lua_State* L)
{
    luaGetPhongMaterial();

    lua_pushnumber(L, glm::sqrt(1.0f / material->shininess));

    return 1;
}

int PhongMaterial::lua_setAmbient(lua_State* L)
{
    luaGetPhongMaterial();

    glm::vec3 ambient;
    luaGetVec3(ambient, 2);

    material->ambient = ambient;

    return 0;
}

int PhongMaterial::lua_setDiffuse(lua_State* L)
{
    luaGetPhongMaterial();

    glm::vec3 diffuse;
    luaGetVec3(diffuse, 2);

    material->diffuse = diffuse;

    return 0;
}

int PhongMaterial::lua_setSpecular(lua_State* L)
{
    luaGetPhongMaterial();

    glm::vec3 specular;
    luaGetVec3(specular, 2);

    material->specular = specular;

    return 0;
}

int PhongMaterial::lua_setShininess(lua_State* L)
{
    luaGetPhongMaterial();

    float shininess;
    luaGet(shininess, float, number, 2);

    material->shininess = shininess;

    return 0;
}

int PhongMaterial::lua_setRoughness(lua_State* L)
{
    luaGetPhongMaterial();

    float roughness;
    luaGet(roughness, float, number, 2);

    material->roughnessToShininess(roughness);

    return 0;
}

#ifndef EMSCRIPTEN
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
#else
const char* PhongShader::m_vertexShaderSource =
    "#version 300 es\n"
    "in mediump vec3 inPos;\n"
    "in mediump vec2 inUV;\n"
    "in mediump vec3 inNormal;\n"
    "in mediump vec4 inColor;\n"
    "out mediump vec2 uv;\n"
    "out mediump vec4 color;\n"
    "out mediump vec3 normal;\n"
    "out mediump vec3 fragPos;\n"
    "uniform mediump mat4 u_projectionMatrix;\n"
    "uniform mediump mat4 u_viewMatrix;\n"
    "uniform mediump mat4 u_modelViewMatrix;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_projectionMatrix * u_modelViewMatrix * vec4(inPos, 1);\n"
    "    uv = inUV;\n"
    "    color = inColor;\n"
    "    normal = inNormal;\n"
    "    fragPos = vec3(u_modelViewMatrix * vec4(inPos, 1.0));\n"
    "}\n";

const char* PhongShader::m_fragmentShaderSource =
    "#version 300 es\n"
    "in mediump vec2 uv;\n"
    "in mediump vec3 normal;\n"
    "in mediump vec4 color;\n"
    "in mediump vec3 fragPos;\n"
    "out mediump vec4 outColor;\n"
    "uniform sampler2D u_textures[8];\n"
    "uniform int u_textureCount;\n"
    "uniform int u_enabledTexture;\n"
    "uniform mediump vec4 u_color;\n"
    "uniform bool u_vertexColorEnabled;\n"
    "uniform mediump vec3 u_cameraPos;\n"
    "struct Material\n"
    "{\n"
    "    mediump vec3 ambient;\n"
    "    mediump vec3 diffuse;\n"
    "    mediump vec3 specular;\n"
    "    mediump float shininess;\n"
    "};\n"
    "uniform Material u_material;\n"
    "struct Light\n"
    "{\n"
    "    mediump vec3 position;\n"
    "    mediump vec3 emission;\n"
    "};\n"
    "uniform Light u_lights[8];\n"
    "uniform int u_lightCount;\n"
    "mediump vec3 calcLight(Light light, mediump vec3 viewDir)\n"
    "{\n"
    "    mediump float distance = length(light.position - fragPos);\n"
    "    mediump vec3 lightDir = normalize(light.position - fragPos);\n"
    "    mediump float diff = max(dot(normal, lightDir), 0.0);\n"
    "    mediump vec3 reflectDir = reflect(-lightDir, normal);\n"
    "    mediump float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);\n"
    "    mediump vec3 ambient = light.emission * u_material.ambient;\n"
    "    mediump vec3 diffuse = light.emission * diff * u_material.diffuse / (distance * distance + 1.0);\n"
    "    mediump vec3 specular = light.emission * spec * u_material.specular;\n"
    "    return (ambient + diffuse + specular);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    outColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "    mediump vec3 viewDir = normalize(u_cameraPos - fragPos);\n"
    "    for(int i = 0; i < u_lightCount; i++)\n"
    "    {\n"
    "        outColor += vec4(calcLight(u_lights[i], viewDir), 0.0);\n"
    "    }\n"
    "    outColor *= u_color;\n"
    "    if(u_vertexColorEnabled) outColor *= color;\n"
    "    if(u_enabledTexture >= 0) outColor *= texture(u_textures[0], uv);\n"
    "}\n";
#endif

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