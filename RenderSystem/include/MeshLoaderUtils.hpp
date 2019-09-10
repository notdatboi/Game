#ifndef MESH_LOADER_UTILS_HPP
#define MESH_LOADER_UTILS_HPP
#include<assimp/scene.h>

class VBGenerator
{
public:
    virtual void generateVertexBuffer(const aiMesh& mesh, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex = ~0) const = 0;  // texture coord index equal to ~0 must be ignored
    virtual const uint32_t getVertexSize() const = 0;
private:
};

class VBGeneratorOptimal : public VBGenerator   // generates "archived" data (uses free bits left after aligning)
{
public:
    virtual void generateVertexBuffer(const aiMesh& mesh, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex = ~0) const;
    virtual const uint32_t getVertexSize() const;
private:
    struct Vertex
    {
        float posAndU[4];
        float normalAndV[4];
    };
};

#endif