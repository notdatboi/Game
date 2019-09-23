#include<Mesh.hpp>

Mesh::Mesh()
{
}

void Mesh::create(ObjectManagementStrategy* allocator, const aiMesh* mesh, const Material* mat)
{
    this->allocator = allocator;
    allocator->allocateVertexBuffer(getVertexBufferSize(), vertexBuffer);
    allocator->allocateIndexBuffer(getIndexBufferSize(), indexBuffer);
    allocator->updateBuffer(tempVertexBuffer->getBufferPtr(), vertexBuffer);
    allocator->updateBuffer(tempIndexBuffer.getPtr(), indexBuffer);
    material = mat;
    generateIndexBuffer(mesh);
    generateVertexBuffer(mesh);
}

const uint32_t Mesh::getIndexBufferSize() const
{
    return sizeof(uint32_t) * tempIndexBuffer.getSize();
}

const uint32_t Mesh::getVertexBufferSize() const
{
    return tempVertexBuffer->getBufferSize();
}

const Material* Mesh::getMaterial() const
{
    return material;
}

void Mesh::generateIndexBuffer(const aiMesh* mesh)
{
    size_t indexBufferIndex = 0;
    for(auto currentFaceId = 0; currentFaceId < mesh->mNumFaces; ++currentFaceId)
    {
        const auto* currentFace = mesh->mFaces + currentFaceId;
        for(auto faceIndexId = 0; faceIndexId < currentFace->mNumIndices; ++faceIndexId)
        {
            tempIndexBuffer[indexBufferIndex] = *(currentFace->mIndices + faceIndexId);
            ++indexBufferIndex;
        }
    }
}

void Mesh::generateVertexBuffer(const aiMesh* mesh)
{
    switch(material->getType())
    {
        case DrawableType::NotTextured:
            tempVertexBuffer = new VertexBufferNotTextured();
            break;
        case DrawableType::Textured:
            tempVertexBuffer = new VertexBufferStandard();
            break;
        case DrawableType::TexturedWithNormalMap:
            tempVertexBuffer = new VertexBufferWithNormalMap();
            break;
    }
    if(!tempVertexBuffer->checkCompatibility(mesh)) reportError("Invalid mesh.\n");
    tempVertexBuffer->loadFromAiMesh(mesh);
}

void Mesh::clearExtraResources()
{
    tempIndexBuffer.clear();
    tempVertexBuffer->clear();
}

void Mesh::destroy()
{
    vertexBuffer = BufferInfo();
    indexBuffer = BufferInfo();
    tempVertexBuffer->clear();
    tempIndexBuffer.clear();
    material = nullptr;
}

Mesh::~Mesh()
{
    destroy();
}