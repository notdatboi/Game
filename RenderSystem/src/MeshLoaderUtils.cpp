#include<MeshLoaderUtils.hpp>
#include<Utils.hpp>

void VBGeneratorOptimal::generateVertexBuffer(const aiMesh& mesh, void* dst, const uint32_t vertexCount, const uint32_t textureCoordIndex) const
{
    if(textureCoordIndex == (~0) || !mesh.HasPositions() || !mesh.HasNormals()) reportError("Invalid mesh.\n");
    Vertex* dstVertices = (Vertex*)dst;
    for(auto ind = 0; ind < vertexCount; ++ind)
    {
        Vertex tmp;
        const aiVector3D& pos = *(mesh.mVertices + ind), & uv = *(mesh.mTextureCoords[textureCoordIndex] + ind), & normal = *(mesh.mNormals + ind);
        tmp.posAndU[0] = pos.x;
        tmp.posAndU[1] = pos.y;
        tmp.posAndU[2] = pos.z;
        tmp.posAndU[3] = uv.x;
        tmp.normalAndV[0] = normal.x;
        tmp.normalAndV[1] = normal.y;
        tmp.normalAndV[2] = normal.z;
        tmp.normalAndV[3] = uv.y;
        memcpy(dstVertices + ind, &tmp, sizeof(Vertex));
    }
}

const uint32_t VBGeneratorOptimal::getVertexSize() const
{
    return sizeof(Vertex);
}