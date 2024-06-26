#include <MeshObject.hpp>

using namespace em;

#include <cstring>

MeshObject::MeshObject(const std::string& name)
    : SceneObject(MESH, name),
    color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

MeshObject::~MeshObject()
{
}

void MeshObject::update(float dt)
{
}

void MeshObject::draw(Shader& shader)
{
    if(!mesh) return;

    if(mesh->getTextureHandle() != 0)
        {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->getTextureHandle());
        shader.setEnabledTexture(0);
    }
    else 
        shader.setEnabledTexture(-1);

    if(strcmp(shader.getName(), "PhongShader") == 0)
    {
        PhongShader& phongShader = static_cast<PhongShader&>(shader);
        phongShader.setMaterial(material);
    }
    
    shader.setColor(color);
    shader.use();
    mesh->render(GL_TRIANGLES);
    
}

void MeshObject::setMesh(Mesh::Ptr mesh)
{
    this->mesh = mesh;
}

Mesh::Ptr MeshObject::getMesh() const
{
    return mesh;
}

void MeshObject::setMaterial(const PhongMaterial& material)
{
    this->material = material;
}

PhongMaterial MeshObject::getMaterial() const
{
    return material;
}

void MeshObject::setColor(const glm::vec4& color)
{
    this->color = color;
}

glm::vec4 MeshObject::getColor() const
{
    return color;
}

#define luaGetMeshObject() \
    MeshObject* meshObject; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(meshObject, MeshObject, -1);

int MeshObject::lua_this(lua_State* L)
{
    if(hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);
    lua_pushstring(L, "transform");
    getTransform().lua_this(L);
    lua_settable(L, -3);
    lua_pushstring(L, "dynamics");
    getDynamics().lua_this(L);
    lua_settable(L, -3);
    lua_pushstring(L, "material");
    material.lua_this(L);
    lua_settable(L, -3);

    luaL_newmetatable(L, "MeshObject");
    lua_setmetatable(L, -2);

    luaRegisterInstance(L);

    return 1;
}

int MeshObject::lua_openMeshObjectLib(lua_State* L)
{
    static const luaL_Reg luaMeshObjectMethods[] = {
        { "getMesh", lua_getMesh },
        { "setMesh", lua_setMesh },
        { "getColor", lua_getColor },
        { "setColor", lua_setColor },
        { nullptr, nullptr }
    };

    PhongMaterial::lua_openPhongMaterialLib(L);

    luaL_newmetatable(L, "MeshObject");
    luaL_setfuncs(L, luaMeshObjectMethods, 0);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    luaL_newmetatable(L, "SceneObject");
    lua_setmetatable(L, -2);

    lua_setglobal(L, "MeshObject");

    return 0;
}

int MeshObject::lua_getMesh(lua_State* L)
{
    luaGetMeshObject();
    luaPushSharedPtr<Mesh>(L, meshObject->mesh, "MeshPtr");

    return 1;
}

int MeshObject::lua_setMesh(lua_State* L)
{
    luaGetMeshObject();
    luaGetSharedPtr<Mesh>(L, meshObject->mesh, "MeshPtr", 2);

    return 0;
}

int MeshObject::lua_getColor(lua_State* L)
{
    luaGetMeshObject();
    luaPushVec4(meshObject->color);

    return 1;
}

int MeshObject::lua_setColor(lua_State* L)
{
    luaGetMeshObject();
    glm::vec4 color;
    luaGetVec4(color, 2);
    meshObject->color = color;

    return 0;
}