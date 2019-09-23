#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include<Constants.hpp>
#include<optional>
#include<string>
#include<Utils.hpp>
#include<ImageHolder.hpp>
#include<ImageLoader.hpp>
#include<assimp/material.h>
#include<ObjectManagementStrategy.hpp>

class Material
{
public:
    enum Descriptors{Colors, Texture, NormalMap};
    Material();
    void create(ObjectManagementStrategy* allocator, const aiMaterial* mat, const std::string& pathToTextures = "");
    const DrawableType getType() const;
    const ImageLoader::Image& getTextureImage() const;
    const ImageLoader::Image& getNormalMapImage() const;
    const Array<DescriptorInfo>& getDescriptorInfos() const;
    void clearExtraResources();
    void destroy();
    ~Material();
private:
    struct Colors
    {   
        float ambientColor[4];
        float diffuseColor[4];
        float specularColor[4];
    } colors;
    struct TempImages
    {
        std::optional<ImageLoader::Image> texture;
        std::optional<ImageLoader::Image> normalMap;
    } tempImages;

    const bool hasTexture() const;
    const bool hasNormalMap() const;

    ObjectManagementStrategy* allocator;
    DrawableType type;
    Array<DescriptorInfo> descriptorInfos;
    BufferInfo colorsBuffer;
    SampledImageInfo texture;
    SampledImageInfo normalMap;
};

#endif