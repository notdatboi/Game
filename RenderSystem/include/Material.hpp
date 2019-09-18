#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include<optional>
#include<string>
#include<Utils.hpp>
#include<ImagePool.hpp>
#include<ImageLoader.hpp>
#include<assimp/material.h>

class Material
{
public:
    Material();
    void create(const aiMaterial* mat, const std::string& pathToTextures = "");
    const uint32_t getColorsSize() const;
    void writeColors(void* dst) const;
    const bool hasTexture() const;
    const bool hasNormalMap() const;
    void createTextureImage(ImagePool* dstPool, const uint32_t dstPoolIndex) const;
    void createNormalMapImage(ImagePool* dstPool, const uint32_t dstPoolIndex) const;
    void writeTexture(void* dst) const;
    void writeNormalMap(void* dst) const;
    void bindDescriptorSets(const Array<uint32_t>& indices);
    void bindDescriptorSets(Array<uint32_t>&& indices);
    const Array<uint32_t>& getDescriptorSetIndices() const;
    void destroy();
    ~Material();
private:
    struct Colors
    {   
        float ambientColor[4];
        float diffuseColor[4];
        float specularColor[4];
    } colors;
    struct Images
    {
        std::optional<ImageLoader::Image> texture;
        std::optional<ImageLoader::Image> normalMap;
    } images;

    void createImage(const ImageLoader::Image& img, ImagePool* dstPool, const uint32_t dstPoolIndex) const;
    Array<uint32_t> descriptorSetIndices;
};

#endif