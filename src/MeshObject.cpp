#include <MeshObject.hpp>

using namespace em;

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

    if(mesh)
    {
        if(mesh->getTextureHandle() != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh->getTextureHandle());
            shader.setEnabledTexture(0);
        }
        else 
            shader.setEnabledTexture(-1);

        shader.use();
        mesh->render(GL_TRIANGLES);
    }
}

void MeshObject::setMesh(Mesh::Ptr mesh)
{
    this->mesh = mesh;
}

Mesh::Ptr MeshObject::getMesh() const
{
    return mesh;
}