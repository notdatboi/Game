#include<Scene.hpp>

Scene::Node::Node(): modelMatrix(1.0f) {}

void Scene::Node::create(const uint32_t maxMeshCount)
{
    meshes.create(maxMeshCount);
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
    node.create(ainode->mNumMeshes);
    for(auto i = 0; i < ainode->mNumMeshes; ++i)
    {
        node.setMesh(i, &meshes[*(ainode->mMeshes + i)]);
    }
    for(auto i = 0; i < ainode->mNumChildren; ++i)
    {
        node.addChild((*(ainode->mChildren + i))->mName.C_Str());
        loadNode(*(ainode->mChildren + i), node[(*(ainode->mChildren + i))->mName.C_Str()]);
    }
}

void Scene::loadFromFile(const std::string& imagePath, const std::string& file)
{
    importedScene = importer.ReadFile(file, 0);
    meshes.create(importedScene->mNumMeshes);
    materials.create(importedScene->mNumMaterials);
    loadMaterials(imagePath);
    loadMeshes();
    //root.create(scene.mRootNode->mNumMeshes);
    loadNode(importedScene->mRootNode, root);
}

Material& Scene::getMaterial(const uint32_t index)
{
    return materials[index];
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