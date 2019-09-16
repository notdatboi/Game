#ifndef MESH_HPP
#define MESH_HPP
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<Material.hpp>
#include<Utils.hpp>

class Mesh
{
public:
    Mesh();
    void create(const uint32_t childrenCount);
    void setChild(const uint32_t index, Mesh* child);
    //void rotate();
    //void scale();
    //void move();
    const glm::mat4& getModelMatrix() const;
    void setMaterial(const Material* mat);
    void setVertexBuffer(BufferInfo&& info);
    void setVertexBuffer(const BufferInfo& info);
    void setIndexBuffer(BufferInfo&& info);
    void setIndexBuffer(const BufferInfo& info);
    void destroy();
    ~Mesh();
private:
    glm::mat4 model;
    const Material* material;
    Array<Mesh*> children;
    BufferInfo vertexBuffer;
    BufferInfo indexBuffer;
};

#endif