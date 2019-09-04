#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP
#include<MeshLoaderUtils.hpp>

class MeshLoader
{
public:
    MeshLoader();
    const uint32_t getApproximateIndexCount(const aiMesh& mesh) const;         // both return the same values if amount of vertices per face is a constant value
    const uint32_t getPreciseIndexCount(const aiMesh& mesh) const;             // this is much slower
    const uint32_t getVertexCount(const aiMesh& mesh) const;
    void generateIndexBuffer(const aiMesh& mesh, void* dst) const;   // indices are assumed to be 32-bit values
    void generateVertexBuffer(const aiMesh& mesh, const VBGenerator* generator, void* dst, const uint32_t vertexCount = 0, const uint32_t textureCoordIndex = ~0); // if vertexCount is 0, then getVertexCount() is called
    const bool hasPositions(const aiMesh& mesh) const;
    const bool hasNormals(const aiMesh& mesh) const;
    const bool hasTangentsAndBitangents(const aiMesh& mesh) const;
    const uint32_t getFirstTextureCoordIndex(const aiMesh& mesh) const;
    ~MeshLoader();
private:
};

#endif