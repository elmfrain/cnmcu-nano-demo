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

    private:
        Mesh::Ptr mesh;
        PhongMaterial material;
    };
}