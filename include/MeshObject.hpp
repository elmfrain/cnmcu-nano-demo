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

        virtual void draw(Shader& shader) override;

        void setMesh(Mesh::Ptr mesh);
        Mesh::Ptr getMesh() const;

        void setMaterial(const PhongMaterial& material);
        PhongMaterial getMaterial() const;

        void setColor(const glm::vec4& color);
        glm::vec4 getColor() const;

        int lua_this(lua_State* L) override;
        static int lua_openMeshObjectLib(lua_State* L);
    private:
        Mesh::Ptr mesh;
        PhongMaterial material;
        glm::vec4 color;

        static int lua_getMesh(lua_State* L);
        static int lua_getColor(lua_State* L);

        static int lua_setMesh(lua_State* L);
        static int lua_setColor(lua_State* L);
    protected:
        virtual void update(float dt) override;
    };
}