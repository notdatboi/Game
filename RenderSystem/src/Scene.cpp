#include<Scene.hpp>
#include<glm/gtx/euler_angles.hpp>

Scene::Node::Node(): modelMatrix(1.0f) {}

void Scene::Node::create(ObjectManagementStrategy* allocator, const uint32_t maxMeshCount)
{
    meshes.create(maxMeshCount);
    allocator->allocateUniformBuffer(sizeof(modelMatrix), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, modelMatrixBuffer, modelMatrixDescriptor);
    allocator->updateBuffer(&modelMatrix, modelMatrixBuffer);
}
#include<iostream>
void Scene::Node::setModelMatrix(const aiMatrix4x4& mat)
{
    aiVector3D aiscale, airotation, aiposition;
    mat.Decompose(aiscale, airotation, aiposition);
    glm::vec3 s = {aiscale.x, aiscale.y, aiscale.z}, r = {airotation.x, airotation.y, airotation.z}, p = {aiposition.x, aiposition.y, aiposition.z};
    std::cout << s.x << ' ' << s.y << ' ' << s.z << " <- scale\n";
    std::cout << p.x << ' ' << p.y << ' ' << p.z << " <- pos\n";
    std::cout << r.x << ' ' << r.y << ' ' << r.z << " <- rot\n";
    modelMatrix *= glm::orientate4(r);
    modelMatrix = glm::translate(modelMatrix, p);
    modelMatrix = glm::scale(modelMatrix, s);
    std::cout << "Model: \n";
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            std::cout << modelMatrix[i][j] << ' ';
        }
        std::cout << '\n';
    }
}

void Scene::Node::setMesh(const uint32_t index, Mesh* mesh)
{
    meshes[index] = mesh;
}

void Scene::Node::addChild(const std::string& name)
{
    children[name] = Node();
}

void Scene::Node::addChild(const std::string& name, Node&& node)
{
    children[name] = node;
}

Scene::Node& Scene::Node::operator[](const std::string& key)
{
    return children[key];
}

const Scene::Node& Scene::Node::operator[](const std::string& key) const
{
    return children.at(key);
}

const Array<Mesh*>& Scene::Node::getMeshes() const
{
    return meshes;
}

const std::map<std::string, Scene::Node>& Scene::Node::getChildrenNodes() const
{
    return children;
}

const DescriptorInfo& Scene::Node::getModelMatrixDescriptor() const
{
    return modelMatrixDescriptor;
}

void Scene::Node::destroy()
{
    modelMatrix = glm::mat4(1.0f);
    meshes.clear();
    children.clear();
}

Scene::Node::~Node()
{
    destroy();
}

Scene::Scene()
{

}

void Scene::setAllocator(ObjectManagementStrategy* allocator)
{
    this->allocator = allocator;
}

void Scene::loadNode(const aiNode* ainode, Node& node)
{
    node.create(allocator, ainode->mNumMeshes);
    node.setModelMatrix(ainode->mTransformation);
    for(auto i = 0; i < ainode->mNumMeshes; ++i)
    {
        node.setMesh(i, &meshes[*(ainode->mMeshes + i)]);
    }
    for(auto i = 0; i < ainode->mNumChildren; ++i)
    {
        std::cout << (*(ainode->mChildren + i))->mName.C_Str() << " <- child\n";
        node.addChild((*(ainode->mChildren + i))->mName.C_Str());
        loadNode(*(ainode->mChildren + i), node[(*(ainode->mChildren + i))->mName.C_Str()]);
    }
}

void Scene::loadFromFile(const std::string& imagePath, const std::string& file)
{
    importedScene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    meshes.create(importedScene->mNumMeshes);
    materials.create(importedScene->mNumMaterials);
    loadMaterials(imagePath);
    loadMeshes();
    loadNode(importedScene->mRootNode, root);
}

Material& Scene::getMaterial(const uint32_t index)
{
    return materials[index];
}

const Scene::Node& Scene::getRootNode() const
{
    return root;
}

Scene::Node& Scene::getRootNode()
{
    return root;
}

Mesh& Scene::getMesh(const uint32_t index)
{
    return meshes[index];
}

Scene::Node& Scene::operator[](const std::string& key)
{
    return root[key];
}

const Scene::Node& Scene::operator[](const std::string& key) const
{
    return root[key];
}

void Scene::loadMaterials(const std::string& imagePath)
{
    for(auto ind = 0; ind < importedScene->mNumMaterials; ++ind)
    {
        materials[ind].create(allocator, *(importedScene->mMaterials + ind), imagePath);
    }
}

void Scene::clearExtraResources()
{
    for(auto ind = 0; ind < materials.getSize(); ++ind) materials[ind].clearExtraResources();
    for(auto ind = 0; ind < meshes.getSize(); ++ind) meshes[ind].clearExtraResources();
}

void Scene::loadMeshes()
{
    for(auto ind = 0; ind < importedScene->mNumMeshes; ++ind)
    {
        meshes[ind].create(allocator, *(importedScene->mMeshes + ind), &materials[(*(importedScene->mMeshes + ind))->mMaterialIndex]);
    }
}

void Scene::destroy()
{
    importer.FreeScene();
    root.destroy();
    materials.clear();
    meshes.clear();
}

Scene::~Scene()
{
    destroy();
}