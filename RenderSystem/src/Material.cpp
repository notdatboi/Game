#include<Material.hpp>

MaterialLoader::MaterialLoader(){}

void MaterialLoader::create(const aiMaterial& mat, const std::string& modelPath)
{
    aiColor3D amb, diff, spec;
    aiString texturePath, normalMapPath;
    if(mat.Get(AI_MATKEY_COLOR_AMBIENT, amb) != AI_SUCCESS) reportError("Invalid material.\n");
    if(mat.Get(AI_MATKEY_COLOR_DIFFUSE, diff) != AI_SUCCESS) reportError("Invalid material.\n");
    if(mat.Get(AI_MATKEY_COLOR_SPECULAR, spec) != AI_SUCCESS) reportError("Invalid material.\n");
    if(mat.GetTexture(aiTextureType::aiTextureType_AMBIENT, 0, &texturePath) || mat.GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath) || mat.GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &texturePath))
    {
        *images.textureFilename = modelPath + texturePath.C_Str();
    }
    if(mat.GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &normalMapPath))
    {
        *images.normalMapFilename = modelPath + normalMapPath.C_Str();
    }
}

const uint32_t MaterialLoader::getColorsSize() const
{
    return sizeof(Colors);
}

void MaterialLoader::writeColors(void* dst) const
{
    memcpy(dst, &colors, sizeof(colors));
}

const bool MaterialLoader::hasTexture() const
{
    return images.textureFilename.has_value();
}

const bool MaterialLoader::hasNormalMap() const
{
    return images.normalMapFilename.has_value();
}

void MaterialLoader::writeTexture(ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst)
{
    if(!images.textureFilename.has_value()) reportError("No texture provided.\n");
    writeImage(images.textureFilename.value(), loader, dstPool, dstPoolIndex, dst);
}

void MaterialLoader::writeNormalMap(ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst)
{
    if(!images.normalMapFilename.has_value()) reportError("No normal map provided.\n");
    writeImage(images.normalMapFilename.value(), loader, dstPool, dstPoolIndex, dst);
}

void MaterialLoader::writeImage(const std::string& file, ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst)
{
    int w, h, c = 4;
    loader->load(file.c_str(), w, h, c);
    c = 4;
    VkExtent3D extent = {w, h, 1};
    memcpy(dst, loader->getData(), extent.width * extent.height * c);
    loader->unload();
    VkFormat format = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
    VkImageTiling tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_BLIT_DST_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_BLIT_SRC_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    if(!dstPool->checkFormatSupport(format, features))
    {
        tiling = VkImageTiling::VK_IMAGE_TILING_LINEAR;
        if(!dstPool->checkFormatSupport(format, features, tiling)) reportError("Image format is not supported.\n");
    }
    dstPool->createImage(dstPoolIndex, format, extent, ImagePool::getMipmapLevelCount(extent), tiling, VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, true, true);
}

void MaterialLoader::destroy()
{
    images.normalMapFilename.reset();
    images.textureFilename.reset();
}

MaterialLoader::~MaterialLoader()
{
    destroy();
}

Material::Material(){}

void Material::bindDescriptorSets(const Array<uint32_t>& indices)
{
    descriptorSetIndices = indices;
}

void Material::bindDescriptorSets(Array<uint32_t>&& indices)
{
    descriptorSetIndices = indices;
}

const Array<uint32_t>& Material::getDescriptorSetIndices() const
{
    return descriptorSetIndices;
}

void Material::destroy()
{
    descriptorSetIndices.clean();
}

Material::~Material()
{
    destroy();
}