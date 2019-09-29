#ifndef MESH_HPP
#define MESH_HPP
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<Material.hpp>
#include<Utils.hpp>
#include<BufferHolder.hpp>
#include<MeshUtils.hpp>
#include<ObjectManagementStrategy.hpp>

class Mesh
{
public:
    Mesh();
    void create(ObjectManagementStrategy* allocator, const aiMesh* mesh, const Material* mat);
    const Material* getMaterial() const;
    const BufferInfo& getVertexBuffer() const;
    const BufferInfo& getIndexBuffer() const;
    const uint32_t getIndexCount() const;
    const uint32_t getVertexCount() const;
    void clearExtraResources();
    void destroy();
    ~Mesh();
private:
    const uint32_t getTempIndexBufferSize() const;
    const uint32_t getTempVertexBufferSize() const;
    void generateTempIndexBuffer(const aiMesh* mesh);   // indices are assumed to be 32-bit values
    void generateTempVertexBuffer(const aiMesh* mesh);
    ObjectManagementStrategy* allocator;
    const Material* material;
    VertexBuffer* tempVertexBuffer;
    Array<uint32_t> tempIndexBuffer;
    BufferInfo vertexBuffer;
    BufferInfo indexBuffer;
    uint32_t vertexCount;
};

#endif