#pragma once

#include <SceneObject.hpp>

namespace em 
{
    class MeshObject : public SceneObject
    {
    public:
        MeshObject(const std::string& name = "MeshObject");
        virtual ~MeshObject();

        virtual void update(float dt) override;
        virtual void draw() override;

    //     void setMesh(Mesh* mesh);
    //     Mesh* getMesh() const;

    //     void setMaterial(Material* material);
    //     Material* getMaterial() const;

    // private:
    //     Mesh* mesh;
    //     Material* material;
    };
}