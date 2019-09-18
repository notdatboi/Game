#include<MeshUtils.hpp>
#include<Utils.hpp>

void VBGeneratorStandard::generateVertexBuffer(const aiMesh& mesh, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex) const
{
    if(textureCoordIndex == (~0) || !mesh.HasPositions() || !mesh.HasNormals() || !mesh.HasTangentsAndBitangents()) reportError("Invalid mesh.\n");
    Vertex* dstVertices = (Vertex*)dst;
    for(auto ind = 0; ind < vertexCount; ++ind)
    {
        Vertex tmp;
        const aiVector3D* pos = mesh.mVertices + ind, * uv = mesh.mTextureCoords[textureCoordIndex] + ind, * normal = mesh.mNormals + ind, * tan = mesh.mTangents + ind, * btan = mesh.mBitangents + ind;
        tmp.posAndU[0] = pos->x;
        tmp.posAndU[1] = pos->y;
        tmp.posAndU[2] = pos->z;
        tmp.posAndU[3] = uv->x;
        tmp.normalAndV[0] = normal->x;
        tmp.normalAndV[1] = normal->y;
        tmp.normalAndV[2] = normal->z;
        tmp.normalAndV[3] = uv->y;
        tmp.tan[0] = tan->x;
        tmp.tan[1] = tan->y;
        tmp.tan[2] = tan->z;
        tmp.btan[0] = btan->x;
        tmp.btan[1] = btan->y;
        tmp.btan[2] = btan->z;

        memcpy(dstVertices + ind, &tmp, sizeof(Vertex));
    }
}

const uint32_t VBGeneratorStandard::getVertexSize() const
{
    return sizeof(Vertex);
}

void VBGeneratorNotTextured::generateVertexBuffer(const aiMesh& mesh, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex = ~0) const
{
    if(!mesh.HasPositions() || !mesh.HasNormals() || !mesh.HasTangentsAndBitangents()) reportError("Invalid mesh.\n");
    Vertex* dstVertices = (Vertex*)dst;
    for(auto ind = 0; ind < vertexCount; ++ind)
    {
        Vertex tmp;
        const aiVector3D* pos = mesh.mVertices + ind, * normal = mesh.mNormals + ind, * tan = mesh.mTangents + ind, * btan = mesh.mBitangents + ind;
        tmp.posAndNormX[0] = pos->x;
        tmp.posAndNormX[1] = pos->y;
        tmp.posAndNormX[2] = pos->z;
        tmp.posAndNormX[3] = normal->x;
        tmp.tanAndNormY[0] = tan->x;
        tmp.tanAndNormY[1] = tan->y;
        tmp.tanAndNormY[2] = tan->z;
        tmp.tanAndNormY[3] = normal->y;
        tmp.btanAndNormZ[0] = btan->x;
        tmp.btanAndNormZ[1] = btan->y;
        tmp.btanAndNormZ[2] = btan->z;
        tmp.btanAndNormZ[3] = normal->z;

        memcpy(dstVertices + ind, &tmp, sizeof(Vertex));
    }
}

const uint32_t VBGeneratorNotTextured::getVertexSize() const
{
    return sizeof(Vertex);
}

void VBGeneratorWithNormalMap::generateVertexBuffer(const aiMesh& mesh, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex = ~0) const
{
    if(textureCoordIndex == (~0) || !mesh.HasPositions() || !mesh.HasTangentsAndBitangents()) reportError("Invalid mesh.\n");
    Vertex* dstVertices = (Vertex*)dst;
    for(auto ind = 0; ind < vertexCount; ++ind)
    {
        Vertex tmp;
        const aiVector3D* pos = mesh.mVertices + ind, * uv = mesh.mTextureCoords[textureCoordIndex] + ind, * tan = mesh.mTangents + ind, * btan = mesh.mBitangents + ind;
        tmp.pos[0] = pos->x;
        tmp.pos[1] = pos->y;
        tmp.pos[2] = pos->z;
        tmp.tanAndU[0] = tan->x;
        tmp.tanAndU[1] = tan->y;
        tmp.tanAndU[2] = tan->z;
        tmp.tanAndU[3] = uv->x;
        tmp.btanAndV[0] = btan->x;
        tmp.btanAndV[1] = btan->y;
        tmp.btanAndV[2] = btan->z;
        tmp.btanAndV[3] = uv->y;

        memcpy(dstVertices + ind, &tmp, sizeof(Vertex));
    }
}

const uint32_t VBGeneratorWithNormalMap::getVertexSize() const
{
    return sizeof(Vertex);
}