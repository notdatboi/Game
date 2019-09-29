#include<Material.hpp>

Material::Material(){}

void Material::create(ObjectManagementStrategy* allocator, const aiMaterial* mat, const std::string& pathToTextures)
{
    this->allocator = allocator;
    aiColor3D amb, diff, spec;
    aiString texturePath, normalMapPath;
    if(mat->Get(AI_MATKEY_COLOR_AMBIENT, amb) != aiReturn_SUCCESS) reportError("Invalid material.\n");
    if(mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff) != aiReturn_SUCCESS) reportError("Invalid material.\n");
    if(mat->Get(AI_MATKEY_COLOR_SPECULAR, spec) != aiReturn_SUCCESS) reportError("Invalid material.\n");
    colors.ambientColor[0] = amb.r;
    colors.ambientColor[1] = amb.g;
    colors.ambientColor[2] = amb.b;
    colors.ambientColor[3] = 1;
    colors.diffuseColor[0] = diff.r;
    colors.diffuseColor[1] = diff.g;
    colors.diffuseColor[2] = diff.b;
    colors.diffuseColor[3] = 1;
    colors.specularColor[0] = spec.r;
    colors.specularColor[1] = spec.g;
    colors.specularColor[2] = spec.b;
    colors.specularColor[3] = 1;
    if(mat->GetTexture(aiTextureType::aiTextureType_AMBIENT, 0, &texturePath) == aiReturn_SUCCESS || mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS || mat->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &texturePath) == aiReturn_SUCCESS)
    {
        tempImages.texture.emplace();
        tempImages.texture.value().load((pathToTextures + texturePath.C_Str()).c_str(), 4);
    }
    if(mat->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &normalMapPath) == aiReturn_SUCCESS)
    {
        tempImages.normalMap.emplace();
        tempImages.normalMap.value().load((pathToTextures + normalMapPath.C_Str()).c_str(), 4);
    }
    descriptorInfos.create(1 + hasTexture() + hasNormalMap());
    allocator->allocateUniformBuffer(sizeof(colors), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, colorsBuffer, descriptorInfos[Descriptors::Colors]);
    allocator->updateBuffer(&colors, colorsBuffer);
    if(hasTexture())
    {
        VkExtent3D extent = {tempImages.texture->getExtent().width, tempImages.texture->getExtent().height, 1};
        allocator->allocateSampledImage(extent, texture, descriptorInfos[Descriptors::Texture]);
        allocator->updateImage(*(tempImages.texture), texture.image);
        if(hasNormalMap())
        {
            VkExtent3D extent = {tempImages.normalMap->getExtent().width, tempImages.normalMap->getExtent().height, 1};
            allocator->allocateSampledImage(extent, normalMap, descriptorInfos[Descriptors::NormalMap]);
            allocator->updateImage(*(tempImages.normalMap), normalMap.image);
            type = DrawableType::DTTexturedWithNormalMap;
        }
        else type = DrawableType::DTTextured;
    }
    else type = DrawableType::DTNotTextured;
}

const DrawableType Material::getType() const
{
    return type;
}

const bool Material::hasTexture() const
{
    return tempImages.texture.has_value();
}

const bool Material::hasNormalMap() const
{
    return tempImages.normalMap.has_value();
}

const ImageLoader::Image& Material::getTextureImage() const
{
    return *tempImages.texture;
}

const ImageLoader::Image& Material::getNormalMapImage() const
{
    return *tempImages.normalMap;
}

void Material::clearExtraResources()
{
    if(tempImages.texture.has_value())
    {
        tempImages.texture.value().unload();
    }
    if(tempImages.normalMap.has_value())
    {
        tempImages.normalMap.value().unload();
    }
}

void Material::destroy()
{
    descriptorInfos.clear();
    tempImages.normalMap.reset();
    tempImages.texture.reset();
}

Material::~Material()
{
    destroy();
}

const Array<DescriptorInfo>& Material::getDescriptorInfos() const
{
    return descriptorInfos;
}