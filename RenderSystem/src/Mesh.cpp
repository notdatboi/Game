#include<Mesh.hpp>

Mesh::Mesh()
{
}

void Mesh::setMaterial(const Material* mat)
{
    material = mat;
}

const uint32_t Mesh::getApproximateIndexCount() const
{
    return aimesh->mNumFaces * aimesh->mFaces->mNumIndices;
}

const uint32_t Mesh::getPreciseIndexCount() const
{
    uint32_t indexCount;
    for(auto currentFaceId = 0; currentFaceId < aimesh->mNumFaces; ++currentFaceId)
    {
        const auto& face = *(aimesh->mFaces + currentFaceId);
        indexCount += face.mNumIndices;
    }
    return indexCount;
}

const uint32_t Mesh::getVertexCount() const
{
    return aimesh->mNumVertices;
}

void Mesh::generateIndexBuffer(void* dst) const
{
    uint32_t* dstChar = (uint32_t*)dst;

    size_t indexBufferIndex = 0;
    for(auto currentFaceId = 0; currentFaceId < aimesh->mNumFaces; ++currentFaceId)
    {
        const auto& currentFace = *(aimesh->mFaces + currentFaceId);
        for(auto faceIndexId = 0; faceIndexId < currentFace.mNumIndices; ++faceIndexId)
        {
            memcpy(dstChar + indexBufferIndex, currentFace.mIndices + faceIndexId, sizeof(uint32_t));
            ++indexBufferIndex;
        }
    }
}

void Mesh::generateVertexBuffer(const VBGenerator* generator, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex) const
{
    generator->generateVertexBuffer(*aimesh, dst, vertexCount ? vertexCount : getVertexCount(), (textureCoordIndex == (~0)) ? getFirstTextureCoordIndex() : textureCoordIndex);
}

const bool Mesh::hasPositions() const
{
    return aimesh->HasPositions();
}

const bool Mesh::hasNormals() const
{
    return aimesh->HasNormals();
}

const bool Mesh::hasTangentsAndBitangents() const
{
    return aimesh->HasTangentsAndBitangents();
}

const uint32_t Mesh::getFirstTextureCoordIndex() const
{
    for(auto index = 0; index < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++index)
    {
        if(aimesh->HasTextureCoords(index))
        {
            return index;
            break;
        }
    }
    return ~0;
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
    material = nullptr;
}

Mesh::~Mesh()
{
    destroy();
}