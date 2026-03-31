#include "Texture.h"

#include <cassert>

#include <stb_image.h>

#include <vulkan/vulkan.h>

#include "Defs.h"
#include "Utils.h"
#include "VulkanEnums.h"


void rk::Texture::create(const char* texturePath) {
    int width, height;
    auto imageData = stbi_load(texturePath, &width, &height, nullptr, 4);

    if (!imageData)
        assert(false && "failed to load texture image");

    u64 imageSize = width * height * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    utl::createBuffer(imageSize, stagingBuffer, stagingBufferMemory, BufferUsage::TRANSFER_SRC,
        MemoryType::HOST_VISIBLE | MemoryType::HOST_COHERENT);

    utl::copyDataToStagingBuffer(imageSize, stagingBufferMemory, imageData);

    // free data in ram
    stbi_image_free(imageData);
}
