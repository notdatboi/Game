#ifndef MESH_HPP
#define MESH_HPP
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
    void clearExtraResources();
    void destroy();
    ~Mesh();
private:
    const uint32_t getIndexBufferSize() const;
    const uint32_t getVertexBufferSize() const;
    void generateIndexBuffer(const aiMesh* mesh);   // indices are assumed to be 32-bit values
    void generateVertexBuffer(const aiMesh* mesh);
    ObjectManagementStrategy* allocator;
    const Material* material;
    VertexBuffer* tempVertexBuffer;
    Array<uint32_t> tempIndexBuffer;
    BufferInfo vertexBuffer;
    BufferInfo indexBuffer;
};

#endif