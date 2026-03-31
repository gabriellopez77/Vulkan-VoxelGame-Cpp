#include "Ubo.h"

#include <cassert>
#include <cstring>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"
#include "VulkanEnums.h"


void rk::Ubo::create(u64 size) {
    auto logicalDevice = VulkanApp::get()->logicalDevice.get();

    for (size_t i = 0; i < utl::FRAMES_COUNT; i++) {
        utl::createBuffer(size, m_buffers[i], m_buffersMemory[i], BufferUsage::UNIFORM_BUFFER,
            MemoryType::HOST_VISIBLE | MemoryType::HOST_COHERENT);

        vkMapMemory(logicalDevice, m_buffersMemory[i], 0, size, 0, &m_buffersMapped[i]);
    }

    m_size = size;
}

void rk::Ubo::update(u64 offset, u64 size, const void* data) const {
    std::memcpy(static_cast<char*>(m_buffersMapped[VulkanApp::get()->getCurrentFrame()]) + offset, data, size);
}

void rk::Ubo::destroy() {
    auto logicalDevice = VulkanApp::get()->logicalDevice.get();

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        vkUnmapMemory(logicalDevice, m_buffersMemory[i]);
        vkDestroyBuffer(logicalDevice, m_buffers[i], nullptr);
        vkFreeMemory(logicalDevice, m_buffersMemory[i], nullptr);
    }
}