#ifndef SCENE_HPP
#define SCENE_HPP
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>
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
        void create(ObjectManagementStrategy* allocator, const uint32_t maxMeshCount);
        void setMesh(const uint32_t index, Mesh* mesh);
        void addChild(const std::string& name);
        void addChild(const std::string& name, Node&& node);
        Node& operator[](const std::string& key);
        const Node& operator[](const std::string& key) const;
        const Array<Mesh*>& getMeshes() const;
        const DescriptorInfo& getModelMatrixDescriptor() const;
        const std::map<std::string, Node>& getChildrenNodes() const;
        void setModelMatrix(const aiMatrix4x4& mat);
        void rotate(const float radians, const glm::vec3 axis);
        void rotate(const glm::vec3 eulerAngles);
        void scale(const glm::vec3 s);
        void move(const glm::vec3 m);
        void destroy();
        ~Node();
    private:
        glm::mat4 modelMatrix;
        BufferInfo modelMatrixBuffer;
        DescriptorInfo modelMatrixDescriptor;
        Array<Mesh*> meshes;
        std::map<std::string, Node> children;
    };
    Scene();
    void setAllocator(ObjectManagementStrategy* allocator);
    void loadFromFile(const std::string& imagePath, const std::string& file);
    Material& getMaterial(const uint32_t index);
    Mesh& getMesh(const uint32_t index);
    Node& operator[](const std::string& key);
    const Node& operator[](const std::string& key) const;
    const Node& getRootNode() const;
    Node& getRootNode();
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