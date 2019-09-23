#include<MeshUtils.hpp>

const uint32_t VertexBuffer::getFirstTextureCoordIndex(const aiMesh* mesh)
{
    for(auto index = 0; index < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++index)
    {
        if(mesh->HasTextureCoords(index))
        {
            return index;
            break;
        }
    }
    return ~0;
}

const bool VertexBuffer::hasPositions(const aiMesh* mesh)
{
    return mesh->HasPositions();
}

const bool VertexBuffer::hasNormals(const aiMesh* mesh)
{
    return mesh->HasNormals();
}

const bool VertexBuffer::hasTangentsAndBitangents(const aiMesh* mesh)
{
    return mesh->HasTangentsAndBitangents();
}

//standard

const bool VertexBufferStandard::checkCompatibility(const aiMesh* mesh) const
{
    if(VertexBuffer::hasPositions(mesh)
        && VertexBuffer::hasNormals(mesh)
        && VertexBuffer::hasTangentsAndBitangents(mesh)
        && VertexBuffer::getFirstTextureCoordIndex(mesh) != (~0)) 
            return true;
    return false;
}

void VertexBufferStandard::loadFromAiMesh(const aiMesh* mesh)
{
    vertices.create(mesh->mNumVertices);
    auto textureCoordIndex = getFirstTextureCoordIndex(mesh);
    for(auto ind = 0; ind < vertices.getSize(); ++ind)
    {
        const aiVector3D* pos = mesh->mVertices + ind, * uv = mesh->mTextureCoords[textureCoordIndex] + ind, * normal = mesh->mNormals + ind, * tan = mesh->mTangents + ind, * btan = mesh->mBitangents + ind;
        vertices[ind].posAndU[0] = pos->x;
        vertices[ind].posAndU[1] = pos->y;
        vertices[ind].posAndU[2] = pos->z;
        vertices[ind].posAndU[3] = uv->x;
        vertices[ind].normalAndV[0] = normal->x;
        vertices[ind].normalAndV[1] = normal->y;
        vertices[ind].normalAndV[2] = normal->z;
        vertices[ind].normalAndV[3] = uv->y;
        vertices[ind].tan[0] = tan->x;
        vertices[ind].tan[1] = tan->y;
        vertices[ind].tan[2] = tan->z;
        vertices[ind].btan[0] = btan->x;
        vertices[ind].btan[1] = btan->y;
        vertices[ind].btan[2] = btan->z;
    }
}

void* VertexBufferStandard::getBufferPtr()
{
    return vertices.getPtr();
}

const uint32_t VertexBufferStandard::getBufferSize() const
{
    return sizeof(Vertex) * vertices.getSize();
}

const uint32_t VertexBufferStandard::getVertexCount() const
{
    return vertices.getSize();
}

void VertexBufferStandard::getGraphicsPipelineVertexInputState(Array<VkVertexInputBindingDescription>& bindings, Array<VkVertexInputAttributeDescription>& attributes)
{
    bindings.create(1);
    bindings[0] = 
    {
        0,
        sizeof(Vertex),
        VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX
    };
    attributes.create(4);
    attributes[0] = 
    {
        0,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, posAndU)
    };
    attributes[1] = 
    {
        1,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, normalAndV)
    };
    attributes[2] = 
    {
        2,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, tan)
    };
    attributes[3] = 
    {
        3,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, btan)
    };
}

void VertexBufferStandard::clear()
{
    vertices.clear();
}

//not textured

const bool VertexBufferNotTextured::checkCompatibility(const aiMesh* mesh) const
{
    if(VertexBuffer::hasPositions(mesh)
        && VertexBuffer::hasNormals(mesh)
        && VertexBuffer::hasTangentsAndBitangents(mesh)) 
            return true;
    return false;
}

void VertexBufferNotTextured::loadFromAiMesh(const aiMesh* mesh)
{
    vertices.create(mesh->mNumVertices);
    auto textureCoordIndex = getFirstTextureCoordIndex(mesh);
    for(auto ind = 0; ind < vertices.getSize(); ++ind)
    {
        const aiVector3D* pos = mesh->mVertices + ind, * normal = mesh->mNormals + ind, * tan = mesh->mTangents + ind, * btan = mesh->mBitangents + ind;
        vertices[ind].posAndNormX[0] = pos->x;
        vertices[ind].posAndNormX[1] = pos->y;
        vertices[ind].posAndNormX[2] = pos->z;
        vertices[ind].posAndNormX[3] = normal->x;
        vertices[ind].tanAndNormY[0] = tan->x;
        vertices[ind].tanAndNormY[1] = tan->y;
        vertices[ind].tanAndNormY[2] = tan->z;
        vertices[ind].tanAndNormY[3] = normal->y;
        vertices[ind].btanAndNormZ[0] = btan->x;
        vertices[ind].btanAndNormZ[1] = btan->y;
        vertices[ind].btanAndNormZ[2] = btan->z;
        vertices[ind].btanAndNormZ[3] = normal->z;
    }
}

void* VertexBufferNotTextured::getBufferPtr()
{
    return vertices.getPtr();
}

const uint32_t VertexBufferNotTextured::getBufferSize() const
{
    return sizeof(Vertex) * vertices.getSize();
}

const uint32_t VertexBufferNotTextured::getVertexCount() const
{
    return vertices.getSize();
}

void VertexBufferNotTextured::getGraphicsPipelineVertexInputState(Array<VkVertexInputBindingDescription>& bindings, Array<VkVertexInputAttributeDescription>& attributes)
{
    bindings.create(1);
    bindings[0] = 
    {
        0,
        sizeof(Vertex),
        VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX
    };
    attributes.create(3);
    attributes[0] = 
    {
        0,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, posAndNormX)
    };
    attributes[1] = 
    {
        1,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, tanAndNormY)
    };
    attributes[2] = 
    {
        2,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, btanAndNormZ)
    };
}

void VertexBufferNotTextured::clear()
{
    vertices.clear();
}

//normal mapped

const bool VertexBufferWithNormalMap::checkCompatibility(const aiMesh* mesh) const
{
    if(VertexBuffer::hasPositions(mesh)
        && VertexBuffer::hasTangentsAndBitangents(mesh)
        && VertexBuffer::getFirstTextureCoordIndex(mesh) != (~0)) 
            return true;
    return false;
}

void VertexBufferWithNormalMap::loadFromAiMesh(const aiMesh* mesh)
{
    vertices.create(mesh->mNumVertices);
    auto textureCoordIndex = getFirstTextureCoordIndex(mesh);
    for(auto ind = 0; ind < vertices.getSize(); ++ind)
    {
        const aiVector3D* pos = mesh->mVertices + ind, * uv = mesh->mTextureCoords[textureCoordIndex] + ind, * tan = mesh->mTangents + ind, * btan = mesh->mBitangents + ind;
        vertices[ind].pos[0] = pos->x;
        vertices[ind].pos[1] = pos->y;
        vertices[ind].pos[2] = pos->z;
        vertices[ind].tanAndU[0] = tan->x;
        vertices[ind].tanAndU[1] = tan->y;
        vertices[ind].tanAndU[2] = tan->z;
        vertices[ind].tanAndU[3] = uv->x;
        vertices[ind].btanAndV[0] = btan->x;
        vertices[ind].btanAndV[1] = btan->y;
        vertices[ind].btanAndV[2] = btan->z;
        vertices[ind].btanAndV[3] = uv->y;
    }
}

void* VertexBufferWithNormalMap::getBufferPtr()
{
    return vertices.getPtr();
}
const uint32_t VertexBufferWithNormalMap::getBufferSize() const
{
    return sizeof(Vertex) * vertices.getSize();
}

const uint32_t VertexBufferWithNormalMap::getVertexCount() const
{
    return vertices.getSize();
}

void VertexBufferWithNormalMap::getGraphicsPipelineVertexInputState(Array<VkVertexInputBindingDescription>& bindings, Array<VkVertexInputAttributeDescription>& attributes)
{
    bindings.create(1);
    bindings[0] = 
    {
        0,
        sizeof(Vertex),
        VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX
    };
    attributes.create(3);
    attributes[0] = 
    {
        0,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, pos)
    };
    attributes[1] = 
    {
        1,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, tanAndU)
    };
    attributes[2] = 
    {
        2,
        0,
        VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT,
        offsetof(Vertex, btanAndV)
    };
}

void VertexBufferWithNormalMap::clear()
{
    vertices.clear();
}