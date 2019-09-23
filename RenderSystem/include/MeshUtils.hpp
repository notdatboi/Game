#ifndef MESH_UTILS_HPP
#define MESH_UTILS_HPP
#include<assimp/scene.h>
#include<Utils.hpp>

class VertexBuffer
{
public:
    virtual const bool checkCompatibility(const aiMesh* mesh) const = 0;
    virtual void loadFromAiMesh(const aiMesh* mesh) = 0;
    virtual void* getBufferPtr() = 0;
    virtual const uint32_t getBufferSize() const = 0;
    virtual const uint32_t getVertexCount() const = 0;
    virtual void getGraphicsPipelineVertexInputState(Array<VkVertexInputBindingDescription>& bindings, Array<VkVertexInputAttributeDescription>& attributes) = 0;
    virtual void clear() = 0;
    static const uint32_t getFirstTextureCoordIndex(const aiMesh* mesh);
    static const bool hasPositions(const aiMesh* mesh);
    static const bool hasNormals(const aiMesh* mesh);
    static const bool hasTangentsAndBitangents(const aiMesh* mesh);
protected:
};

class VertexBufferStandard : public VertexBuffer
{
public:
    virtual const bool checkCompatibility(const aiMesh* mesh) const;
    virtual void loadFromAiMesh(const aiMesh* mesh);
    virtual void* getBufferPtr();
    virtual const uint32_t getBufferSize() const;
    virtual const uint32_t getVertexCount() const;
    virtual void getGraphicsPipelineVertexInputState(Array<VkVertexInputBindingDescription>& bindings, Array<VkVertexInputAttributeDescription>& attributes);
    virtual void clear();
protected:
    struct Vertex
    {
        float posAndU[4];
        float normalAndV[4];
        float tan[4];
        float btan[4];
    };
    Array<Vertex> vertices;
};

class VertexBufferNotTextured : public VertexBuffer
{
public:
    virtual const bool checkCompatibility(const aiMesh* mesh) const;
    virtual void loadFromAiMesh(const aiMesh* mesh);
    virtual void* getBufferPtr();
    virtual const uint32_t getBufferSize() const;
    virtual const uint32_t getVertexCount() const;
    virtual void getGraphicsPipelineVertexInputState(Array<VkVertexInputBindingDescription>& bindings, Array<VkVertexInputAttributeDescription>& attributes);
    virtual void clear();
protected:
    struct Vertex
    {
        float posAndNormX[4];
        float tanAndNormY[4];
        float btanAndNormZ[4];
    };
    Array<Vertex> vertices;
};

class VertexBufferWithNormalMap : public VertexBuffer
{
public:
    virtual const bool checkCompatibility(const aiMesh* mesh) const;
    virtual void loadFromAiMesh(const aiMesh* mesh);
    virtual void* getBufferPtr();
    virtual const uint32_t getBufferSize() const;
    virtual const uint32_t getVertexCount() const;
    virtual void getGraphicsPipelineVertexInputState(Array<VkVertexInputBindingDescription>& bindings, Array<VkVertexInputAttributeDescription>& attributes);
    virtual void clear();
protected:
    struct Vertex
    {
        float pos[4];
        float tanAndU[4];
        float btanAndV[4];
    };
    Array<Vertex> vertices;
};

#endif