#include<Mesh.hpp>

Mesh::Mesh()
{
}

void Mesh::create(ObjectManagementStrategy* allocator, const aiMesh* mesh, const Material* mat)
{
    this->allocator = allocator;
    material = mat;
    generateTempIndexBuffer(mesh);
    generateTempVertexBuffer(mesh);
    vertexCount = tempVertexBuffer->getVertexCount();
    allocator->allocateVertexBuffer(getTempVertexBufferSize(), vertexBuffer);
    allocator->allocateIndexBuffer(getTempIndexBufferSize(), indexBuffer);
    allocator->updateBuffer(tempVertexBuffer->getBufferPtr(), vertexBuffer);
    allocator->updateBuffer(tempIndexBuffer.getPtr(), indexBuffer);
}

const BufferInfo& Mesh::getVertexBuffer() const
{
    return vertexBuffer;
}

const BufferInfo& Mesh::getIndexBuffer() const
{
    return indexBuffer;
}

const uint32_t Mesh::getIndexCount() const
{
    return indexBuffer.size / sizeof(uint32_t);
}

const uint32_t Mesh::getVertexCount() const
{
    return vertexCount;
}

const uint32_t Mesh::getTempIndexBufferSize() const
{
    return sizeof(uint32_t) * tempIndexBuffer.getSize();
}

const uint32_t Mesh::getTempVertexBufferSize() const
{
    return tempVertexBuffer->getBufferSize();
}

const Material* Mesh::getMaterial() const
{
    return material;
}

void Mesh::generateTempIndexBuffer(const aiMesh* mesh)
{
    static const uint32_t INDICES_PER_FACE = 3;
    tempIndexBuffer.create(INDICES_PER_FACE * mesh->mNumFaces);
    size_t indexBufferIndex = 0;
    for(auto currentFaceId = 0; currentFaceId < mesh->mNumFaces; ++currentFaceId)
    {
        const auto* currentFace = mesh->mFaces + currentFaceId;
        for(auto faceIndexId = 0; faceIndexId < INDICES_PER_FACE; ++faceIndexId)
        {
            tempIndexBuffer[indexBufferIndex] = *(currentFace->mIndices + faceIndexId);
            ++indexBufferIndex;
        }
    }
}

void Mesh::generateTempVertexBuffer(const aiMesh* mesh)
{
    switch(material->getType())
    {
        case DrawableType::DTNotTextured:
            tempVertexBuffer = new VertexBufferNotTextured();
            break;
        case DrawableType::DTTextured:
            tempVertexBuffer = new VertexBufferStandard();
            break;
        case DrawableType::DTTexturedWithNormalMap:
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
    delete tempVertexBuffer;
    tempIndexBuffer.clear();
    material = nullptr;
}

Mesh::~Mesh()
{
    destroy();
}