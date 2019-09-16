#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include<optional>
#include<string>
#include<Utils.hpp>
#include<ImagePool.hpp>
#include<ImageLoader.hpp>
#include<assimp/material.h>

class MaterialLoader
{
public:
    MaterialLoader();
    void create(const aiMaterial& mat, const std::string& modelPath = "");
    const uint32_t getColorsSize() const;
    void writeColors(void* dst) const;
    const bool hasTexture() const;
    const bool hasNormalMap() const;
    void writeTexture(ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst);     // creates image, loads its data to dst
    void writeNormalMap(ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst);
    void destroy();
    ~MaterialLoader();
private:
    struct Colors
    {   
        float ambientColor[4];
        float diffuseColor[4];
        float specularColor[4];
    } colors;
    struct Images
    {
        std::optional<std::string> textureFilename;
        std::optional<std::string> normalMapFilename;
    } images;

    void writeImage(const std::string& file, ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst);
};

class Material
{
public:
    Material();
    void bindDescriptorSets(const Array<uint32_t>& indices);
    void bindDescriptorSets(Array<uint32_t>&& indices);
    const Array<uint32_t>& getDescriptorSetIndices() const;
    void destroy();
    ~Material();
private:
    Array<uint32_t> descriptorSetIndices;
};

#endif