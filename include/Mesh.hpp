#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP
#include<assimp/scene.h>
#include<HardwareBuffer.hpp>

namespace spk
{
    class VertexDescription
    {
    public:
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
    };

    class Mesh
    {
    public:
        Mesh();
        void loadFromAssimpMesh(const aiMesh& mesh);        // positions, then normals, then texture coords, then tangents and bitangents; every attribute is vec4
        template<typename Vertex>
        void loadFromVectors(const std::shared_ptr<VertexDescription>& vertexDescription, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();
    private:
        //friend class Pipeline;
        //friend class Renderer;
        const std::shared_ptr<VertexDescription> getVertexDescription() const;
        const HardwareBuffer& getVertexBuffer() const;
        const HardwareBuffer& getIndexBuffer() const;
        HardwareBuffer vertexBuffer;
        HardwareBuffer indexBuffer;
        std::shared_ptr<VertexDescription> vertexDescription;
    };

    template<typename Vertex>
    void Mesh::loadFromVectors(const std::shared_ptr<VertexDescription>& vertexDescription, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        this->vertexDescription = vertexDescription;
        vertexBuffer.setSize(vertices.size() * sizeof(Vertex));
        vertexBuffer.loadFromMemory(vertices.data());
        indexBuffer.setSize(indices.size() * sizeof(uint32_t));
        indexBuffer.loadFromMemory(indices.data());
    }
} 

#endif