#include "Ubo.h"

#include <cassert>
#include <cstring>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"


void rk::Ubo::create(const VulkanApp* app, u64 size) {
    m_size = size;

    for (size_t i = 0; i < utl::FRAMES_COUNT; i++) {
        utl::createBuffer(app, size, &m_buffers[i], &m_buffersMemory[i],
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        vkMapMemory(app->logicalDevice.get(), m_buffersMemory[i], 0, size, 0, &m_buffersMapped[i]);
    }
}

void rk::Ubo::update(u32 currentFrame, u64 offset, u64 size, const void* data) {
    std::memcpy(m_buffersMapped[currentFrame] + offset, data, size);
}

void rk::Ubo::destroy(const VulkanApp* app) {
    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        vkDestroyBuffer(app->logicalDevice.get(), m_buffers[i], nullptr);
        vkFreeMemory(app->logicalDevice.get(), m_buffersMemory[i], nullptr);
    }
}