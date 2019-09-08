#include<MeshLoader.hpp>

MeshLoader::MeshLoader(){}

const uint32_t MeshLoader::getApproximateIndexCount(const aiMesh& mesh) const
{
    return mesh.mNumFaces * mesh.mFaces->mNumIndices;
}

const uint32_t MeshLoader::getPreciseIndexCount(const aiMesh& mesh) const
{
    uint32_t indexCount;
    for(auto currentFaceId = 0; currentFaceId < mesh.mNumFaces; ++currentFaceId)
    {
        const auto& face = *(mesh.mFaces + currentFaceId);
        indexCount += face.mNumIndices;
    }
    return indexCount;
}

const uint32_t MeshLoader::getVertexCount(const aiMesh& mesh) const
{
    return mesh.mNumVertices;
}

void MeshLoader::generateIndexBuffer(const aiMesh& mesh, void* dst) const
{
    uint32_t* dstChar = (uint32_t*)dst;

    size_t indexBufferIndex = 0;
    for(auto currentFaceId = 0; currentFaceId < mesh.mNumFaces; ++currentFaceId)
    {
        const auto& currentFace = *(mesh.mFaces + currentFaceId);
        for(auto faceIndexId = 0; faceIndexId < currentFace.mNumIndices; ++faceIndexId)
        {
            memcpy(dstChar + indexBufferIndex, currentFace.mIndices + faceIndexId, sizeof(uint32_t));
            ++indexBufferIndex;
        }
    }
}

void MeshLoader::generateVertexBuffer(const aiMesh& mesh, const VBGenerator* generator, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex)
{
    generator->generateVertexBuffer(mesh, dst, vertexCount ? vertexCount : getVertexCount(mesh), textureCoordIndex);
}

const bool MeshLoader::hasPositions(const aiMesh& mesh) const
{
    return mesh.HasPositions();
}

const bool MeshLoader::hasNormals(const aiMesh& mesh) const
{
    return mesh.HasNormals();
}

const bool MeshLoader::hasTangentsAndBitangents(const aiMesh& mesh) const
{
    return mesh.HasTangentsAndBitangents();
}

const uint32_t MeshLoader::getFirstTextureCoordIndex(const aiMesh& mesh) const
{
    for(auto index = 0; index < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++index)
    {
        if(mesh.HasTextureCoords(index))
        {
            return index;
            break;
        }
    }
    return ~0;
}

MeshLoader::~MeshLoader(){}