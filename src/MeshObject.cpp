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

void MeshObject::draw()
{
}