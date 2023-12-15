#pragma once

#include <SceneObject.hpp>
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

    //     void setMaterial(Material* material);
    //     Material* getMaterial() const;

    // private:
    //     Mesh* mesh;
    //     Material* material;
    private:
        Mesh::Ptr mesh;
    };
}