#pragma once

#include <SceneObject.hpp>
#include <shaders/PhongShader.hpp>
#include <Mesh.hpp>

namespace em 
{
    class MeshObject : public SceneObject
    {
    public:
        MeshObject(const std::string& name = "MeshObject");
        virtual ~MeshObject();

        virtual void update(float dt) override;
        virtual void draw(Shader& shader) override;

        void setMesh(Mesh::Ptr mesh);
        Mesh::Ptr getMesh() const;

        void setMaterial(const PhongMaterial& material);
        PhongMaterial getMaterial() const;

        int lua_this(lua_State* L) override;
        static int lua_openMeshObjectLib(lua_State* L);
    private:
        Mesh::Ptr mesh;
        PhongMaterial material;

        static int lua_getMesh(lua_State* L);

        static int lua_setMesh(lua_State* L);
    };
}