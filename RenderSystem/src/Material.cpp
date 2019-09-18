#include<Material.hpp>

void Material::create(const aiMaterial* mat, const std::string& pathToTextures)
{
    aiColor3D amb, diff, spec;
    aiString texturePath, normalMapPath;
    if(mat->Get(AI_MATKEY_COLOR_AMBIENT, amb) != AI_SUCCESS) reportError("Invalid material.\n");
    if(mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff) != AI_SUCCESS) reportError("Invalid material.\n");
    if(mat->Get(AI_MATKEY_COLOR_SPECULAR, spec) != AI_SUCCESS) reportError("Invalid material.\n");
    if(mat->GetTexture(aiTextureType::aiTextureType_AMBIENT, 0, &texturePath) || mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath) || mat->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &texturePath))
    {
        images.texture.value().load((pathToTextures + texturePath.C_Str()).c_str(), 4);
    }
    if(mat->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &normalMapPath))
    {
        images.normalMap.value().load((pathToTextures + normalMapPath.C_Str()).c_str(), 4);
    }
}

const uint32_t Material::getColorsSize() const
{
    return sizeof(Colors);
}

void Material::writeColors(void* dst) const
{
    memcpy(dst, &colors, sizeof(colors));
}

const bool Material::hasTexture() const
{
    return images.texture.has_value();
}

const bool Material::hasNormalMap() const
{
    return images.normalMap.has_value();
}

void Material::createTextureImage(ImagePool* dstPool, const uint32_t dstPoolIndex) const
{
    if(!images.texture.has_value()) reportError("No texture provided.\n");
    createImage(images.texture.value(), dstPool, dstPoolIndex);
}

void Material::createNormalMapImage(ImagePool* dstPool, const uint32_t dstPoolIndex) const
{
    if(!images.normalMap.has_value()) reportError("No normal map provided.\n");
    createImage(images.normalMap.value(), dstPool, dstPoolIndex);
}

void Material::writeTexture(void* dst) const
{
    VkExtent2D extent = std::move(images.texture.value().getExtent());
    uint32_t channels = images.texture.value().getChannelCount();
    memcpy(dst, images.texture.value().getData(), extent.width * extent.height * channels);
}

void Material::writeNormalMap(void* dst) const
{
    VkExtent2D extent = std::move(images.normalMap.value().getExtent());
    uint32_t channels = images.normalMap.value().getChannelCount();
    memcpy(dst, images.normalMap.value().getData(), extent.width * extent.height * channels);
}

/*void Material::writeTexture(ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst)
{
    if(!images.textureFilename.has_value()) reportError("No texture provided.\n");
    writeImage(images.textureFilename.value(), loader, dstPool, dstPoolIndex, dst);
}

void Material::writeNormalMap(ImageLoader* loader, ImagePool* dstPool, const uint32_t dstPoolIndex, void* dst)
{
    if(!images.normalMapFilename.has_value()) reportError("No normal map provided.\n");
    writeImage(images.normalMapFilename.value(), loader, dstPool, dstPoolIndex, dst);
}*/

void Material::createImage(const ImageLoader::Image& img, ImagePool* dstPool, const uint32_t dstPoolIndex) const
{
    VkExtent3D extent = {img.getExtent().width , img.getExtent().height, 1};
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

void Material::destroy()
{
    descriptorSetIndices.clean();
    images.normalMap.reset();
    images.texture.reset();
}

Material::~Material()
{
    destroy();
}

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