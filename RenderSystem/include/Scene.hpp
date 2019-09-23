#ifndef SCENE_HPP
#define SCENE_HPP
#include<assimp/Importer.hpp>
#include<Mesh.hpp>
#include<Material.hpp>
#include<map>
#include<string>
#include<assimp/scene.h>

class Scene
{
public:
    class Node
    {
    public:
        Node();
        void create(const uint32_t maxMeshCount);
        void setMesh(const uint32_t index, Mesh* mesh);
        void addChild(const std::string& name);
        void addChild(const std::string& name, Node&& node);
        Node& operator[](const std::string& key);
        const Node& operator[](const std::string& key) const;
        const Array<Mesh*>& getMeshes() const;
        // rotate, scale, move
        void destroy();
        ~Node();
    private:
        glm::mat4 modelMatrix;
        Array<Mesh*> meshes;
        std::map<std::string, Node> children;
    };
    Scene();
    void setAllocator(ObjectManagementStrategy* allocator);
    void loadFromFile(const std::string& imagePath, const std::string& file);
    //void create(const uint32_t maxMeshCount, const uint32_t maxMaterialCount);
    Material& getMaterial(const uint32_t index);
    Mesh& getMesh(const uint32_t index);
    Node& operator[](const std::string& key);
    const Node& operator[](const std::string& key) const;
    void clearExtraResources();
    void destroy();
    ~Scene();
private:
    ObjectManagementStrategy* allocator;
    void loadNode(const aiNode* ainode, Node& node);
    void loadMeshes();
    void loadMaterials(const std::string& imagePath);
    Assimp::Importer importer;
    const aiScene* importedScene;
    Array<Material> materials;
    Array<Mesh> meshes;
    // lights
    Node root;
};

#endif