#include <MeshObject.hpp>

using namespace em;

#include <cstring>

MeshObject::MeshObject(const std::string& name)
    : SceneObject(MESH, name)
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
    shader.setModelViewMatrix(getConstTransform().getMatrix());

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

#define luaGetMeshObject() \
    MeshObject* meshObject; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(meshObject, MeshObject, -1);

int MeshObject::lua_this(lua_State* L)
{
    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);
    lua_pushstring(L, "transform");
    getTransform().lua_this(L);
    lua_settable(L, -3);
    lua_pushstring(L, "material");
    material.lua_this(L);
    lua_settable(L, -3);

    luaL_newmetatable(L, "MeshObject");
    lua_setmetatable(L, -2);

    return 1;
}

int MeshObject::lua_openMeshObjectLib(lua_State* L)
{
    static const luaL_Reg luaMeshObjectMethods[] = {
        { "getMesh", lua_getMesh },
        { "setMesh", lua_setMesh },
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