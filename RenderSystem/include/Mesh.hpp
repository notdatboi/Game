#ifndef MESH_HPP
#define MESH_HPP
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<Material.hpp>
#include<Utils.hpp>
#include<BufferPool.hpp>
#include<MeshLoaderUtils.hpp>

class Mesh
{
public:
    Mesh();
    void setAiMesh(const aiMesh* mesh);
    const uint32_t getApproximateIndexCount() const;         // both return the same values if amount of vertices per face is a constant value
    const uint32_t getPreciseIndexCount() const;             // this is much slower
    const uint32_t getVertexCount() const;
    void generateIndexBuffer(void* dst) const;   // indices are assumed to be 32-bit values
    void generateVertexBuffer(const VBGenerator* generator, void* dst, const uint32_t vertexCount = 0, const uint32_t textureCoordIndex = ~0) const; // if vertexCount is 0, then getVertexCount() is called
    const bool hasPositions() const;
    const bool hasNormals() const;
    const bool hasTangentsAndBitangents() const;
    const uint32_t getFirstTextureCoordIndex() const;
    void setMaterial(const Material* mat);
    void setBufferPool(const BufferPool* pool);
    void setVertexBuffer(BufferInfo&& info);
    void setVertexBuffer(const BufferInfo& info);
    void setIndexBuffer(BufferInfo&& info);
    void setIndexBuffer(const BufferInfo& info);
    void destroy();
    ~Mesh();
private:
    const aiMesh* aimesh;
    const Material* material;
    const BufferPool* pool;
    BufferInfo vertexBuffer;
    BufferInfo indexBuffer;
};

#endif