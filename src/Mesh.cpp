#include<Mesh.hpp>

namespace spk
{
    Mesh::Mesh()
    {
        vertexBuffer.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
        vertexBuffer.setShadowBufferPolicy(true);
        vertexBuffer.setAccessibility(HardwareResourceAccessibility::Static);
        indexBuffer.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
        indexBuffer.setShadowBufferPolicy(true);
        indexBuffer.setAccessibility(HardwareResourceAccessibility::Static);
    }

    void Mesh::loadFromAssimpMesh(const aiMesh& mesh)        // positions, then normals, then texture coords, then tangents and bitangents
    {
        const auto& logicalDevice = spk::system::System::getInstance()->getLogicalDevice();

        {
            vk::VertexInputBindingDescription binding0;
            binding0.setBinding(0)
                .setInputRate(vk::VertexInputRate::eVertex);
            vertexDescription.get()->bindingDescriptions.push_back(binding0);
        }

        uint32_t vertexCount = mesh.mNumVertices;
        uint32_t indexCount = 0;

        const auto faceCount = mesh.mNumFaces;
        for(auto currentFaceId = 0; currentFaceId < faceCount; ++currentFaceId)
        {
            const auto& face = *(mesh.mFaces + currentFaceId);
            indexCount += face.mNumIndices;
        }

        size_t vertexSize = 0;

        int textureCoordsIndex = -1;
        for(auto index = 0; index < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++index)
        {
            if(mesh.HasTextureCoords(index))
            {
                textureCoordsIndex = index;
                break;
            }
        }
        if(mesh.HasPositions())
        {
            vk::VertexInputAttributeDescription positionAttributeDescription;
            positionAttributeDescription.setBinding(0)
                .setFormat(vk::Format::eR8G8B8A8Snorm)
                .setLocation(0)
                .setOffset(0);
            vertexDescription.get()->attributeDescriptions.push_back(positionAttributeDescription);
            vertexSize += sizeof(float) * 4;
            if(mesh.HasNormals())
            {
                vk::VertexInputAttributeDescription normalAttributeDescription;
                normalAttributeDescription.setBinding(0)
                    .setFormat(vk::Format::eR8G8B8A8Snorm)
                    .setLocation(1)
                    .setOffset(vertexSize);
                vertexDescription.get()->attributeDescriptions.push_back(normalAttributeDescription);
                vertexSize += sizeof(float) * 4;
                if(textureCoordsIndex != -1)
                {
                    vk::VertexInputAttributeDescription uvAttributeDescription;
                    uvAttributeDescription.setBinding(0)
                        .setFormat(vk::Format::eR8G8B8A8Snorm)
                        .setLocation(2)
                        .setOffset(vertexSize);
                    vertexDescription.get()->attributeDescriptions.push_back(uvAttributeDescription);
                    vertexSize += sizeof(float) * 4;
                    if(mesh.HasTangentsAndBitangents())
                    {
                        vk::VertexInputAttributeDescription tangentAttributeDescription;
                        tangentAttributeDescription.setBinding(0)
                            .setFormat(vk::Format::eR8G8B8A8Snorm)
                            .setLocation(3)
                            .setOffset(vertexSize);
                        vertexDescription.get()->attributeDescriptions.push_back(tangentAttributeDescription);
                        vertexSize += sizeof(float) * 4;
                        vk::VertexInputAttributeDescription bitangentAttributeDescription;
                        bitangentAttributeDescription.setBinding(0)
                            .setFormat(vk::Format::eR8G8B8A8Snorm)
                            .setLocation(4)
                            .setOffset(vertexSize);
                        vertexDescription.get()->attributeDescriptions.push_back(bitangentAttributeDescription);
                        vertexSize += sizeof(float) * 4;
                    }
                    else
                    {
                        throw std::invalid_argument("Invalid mesh.\n");
                    }
                }
                else
                {
                    throw std::invalid_argument("Invalid mesh.\n");
                }
            }
            else
            {
                throw std::invalid_argument("Invalid mesh.\n");
            }
        }
        else throw std::invalid_argument("Invalid mesh.\n");

        uint32_t stride = vertexSize;
        vertexDescription.get()->bindingDescriptions[0].setStride(stride);

        vertexBuffer.setSize(vertexSize * vertexCount);
        vertexBuffer.load();

        void* vertexBufferDataPtr = vertexBuffer.map();
        char* vertexBufferDataCharPtrCasted = reinterpret_cast<char*>(vertexBufferDataPtr);

        for(int index = 0; index < vertexCount; ++index)
        {
            std::array<float, 4> attribute;
            if(mesh.HasPositions())
            {
                attribute = {(mesh.mVertices + index)->x, (mesh.mVertices + index)->y, (mesh.mVertices + index)->z};    // position
                memcpy(vertexBufferDataCharPtrCasted + stride * index, &attribute, sizeof(attribute));
                if(mesh.HasNormals())
                {
                    attribute = {(mesh.mNormals + index)->x, (mesh.mNormals + index)->y, (mesh.mNormals + index)->z};       // normal
                    memcpy(vertexBufferDataCharPtrCasted + stride * index + sizeof(attribute), &attribute, sizeof(attribute));
                    if(textureCoordsIndex != -1)
                    {
                        attribute = {(mesh.mTextureCoords[textureCoordsIndex] + index)->x, (mesh.mTextureCoords[textureCoordsIndex] + index)->y};   // uv
                        memcpy(vertexBufferDataCharPtrCasted + stride * index + sizeof(attribute) * 2, &attribute, sizeof(attribute));
                        if(mesh.HasTangentsAndBitangents())
                        {
                            attribute = {(mesh.mTangents + index)->x, (mesh.mTangents + index)->y, (mesh.mTangents + index)->z};        // tangent
                            memcpy(vertexBufferDataCharPtrCasted + stride * index + sizeof(attribute) * 3, &attribute, sizeof(attribute));
                            attribute = {(mesh.mBitangents + index)->x, (mesh.mBitangents + index)->y, (mesh.mBitangents + index)->z};
                            memcpy(vertexBufferDataCharPtrCasted + stride * index + sizeof(attribute) * 4, &attribute, sizeof(attribute)); // bitangent
                        }
                    }
                }
            }
        }

        vertexBuffer.unmap();

        indexBuffer.setSize(indexCount * sizeof(uint32_t));
        indexBuffer.load();

        void* indexBufferDataPtr = indexBuffer.map();
        char* indexBufferDataCharPtrCasted = reinterpret_cast<char*>(indexBufferDataPtr);

        size_t indexBufferIndex = 0;
        for(auto currentFaceId = 0; currentFaceId < faceCount; ++currentFaceId)
        {
            const auto& face = *(mesh.mFaces + currentFaceId);
            for(auto faceIndexId = 0; faceIndexId < face.mNumIndices; ++faceIndexId)
            {
                memcpy(indexBufferDataCharPtrCasted + sizeof(uint32_t) * indexBufferIndex, face.mIndices + faceIndexId, sizeof(uint32_t));
                ++indexBufferIndex;
            }
        }

        indexBuffer.unmap();
    }

    const std::shared_ptr<VertexDescription> Mesh::getVertexDescription() const
    {
        return vertexDescription;
    }

    const HardwareBuffer& Mesh::getVertexBuffer() const
    {
        return vertexBuffer;
    }

    const HardwareBuffer& Mesh::getIndexBuffer() const
    {
        return vertexBuffer;
    }

    Mesh::~Mesh(){}
}