#include<Mesh.hpp>

Mesh::Mesh(): model(1.0f)
{
}

void Mesh::create(const uint32_t childrenCount)
{
    children.create(childrenCount);
}

void Mesh::setChild(const uint32_t index, Mesh* child)
{
    children[index] = child;
}

const glm::mat4& Mesh::getModelMatrix() const
{
    return model;
}

void Mesh::setMaterial(const Material* mat)
{
    material = mat;
}

void Mesh::setVertexBuffer(BufferInfo&& info)
{
    vertexBuffer = info;
}

void Mesh::setVertexBuffer(const BufferInfo& info)
{
    vertexBuffer = info;
}

void Mesh::setIndexBuffer(BufferInfo&& info)
{
    indexBuffer = info;
}

void Mesh::setIndexBuffer(const BufferInfo& info)
{
    indexBuffer = info;
}

void Mesh::destroy()
{
    vertexBuffer = BufferInfo();
    indexBuffer = BufferInfo();
    model = glm::mat4(1.0f);
    material = nullptr;
    children.clean();
}

Mesh::~Mesh()
{
    destroy();
}