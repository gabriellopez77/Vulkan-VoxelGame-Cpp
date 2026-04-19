#include "RawTexture.h"

#include <cassert>

#include <stb_image.h>

#include <vulkan/vulkan.h>

#include "Utils.h"
#include "VulkanApp.h"


void rk::RawTexture::create(str texturePath, SamplerFilter filter, SamplerMode mode) {
    int width, height;
    auto imageData = stbi_load(texturePath, &width, &height, nullptr, 4);

    if (!imageData) {
        assert(false && "failed to load texture image");
    }

    create(imageData, width, height, filter, mode);

    // free data in ram
    stbi_image_free(imageData);
}

void rk::RawTexture::create(const void* data, i32 width, i32 height, SamplerFilter filter, SamplerMode mode) {
    u64 imageSize = width * height * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    utl::createBuffer(imageSize, stagingBuffer, stagingBufferMemory, BufferUsage::TransderSrc,
        MemoryType::HostVisible | MemoryType::HostCoherent);

    utl::copyDataToStagingBuffer(imageSize, stagingBufferMemory, data, false);



    utl::createImage(width, height, m_image, m_imageMemory, Formats::Rgba8Srgb, MemoryType::DeviceLocal,
        ImageUsage::TransferDst | ImageUsage::Sampled);


    utl::transitionImageLayout(m_image, Formats::Rgba8Srgb, ImageLayout::Undefined, ImageLayout::TransferDstOptimal);
    utl::copyBufferToImage(width, height, stagingBuffer, m_image);
    utl::transitionImageLayout(m_image, Formats::Rgba8Srgb, ImageLayout::TransferDstOptimal, ImageLayout::ShaderReadOnlyOptimal);

    m_imageView = utl::createImageView(m_image, Formats::Rgba8Srgb, VK_IMAGE_ASPECT_COLOR_BIT);

    createSampler(filter, mode);

    // clear buffer and memory staging
    vkDestroyBuffer(vulkanApp::getLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(vulkanApp::getLogicalDevice(), stagingBufferMemory, nullptr);
}

void rk::RawTexture::destroy() const {
    auto logicalDevice = vulkanApp::getLogicalDevice();

    vkDestroySampler(logicalDevice, m_sampler, nullptr);
    vkDestroyImageView(logicalDevice, m_imageView, nullptr);
    vkDestroyImage(logicalDevice, m_image, nullptr);
    vkFreeMemory(logicalDevice, m_imageMemory, nullptr);
}

void rk::RawTexture::createSampler(SamplerFilter filter, SamplerMode mode) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = static_cast<VkFilter>(filter);
    samplerInfo.minFilter = static_cast<VkFilter>(filter);
    samplerInfo.addressModeU = static_cast<VkSamplerAddressMode>(mode);
    samplerInfo.addressModeV = static_cast<VkSamplerAddressMode>(mode);
    samplerInfo.addressModeW = static_cast<VkSamplerAddressMode>(mode);
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(vulkanApp::getLogicalDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        assert(false && "failed to create texture sampler!");
    }
}
