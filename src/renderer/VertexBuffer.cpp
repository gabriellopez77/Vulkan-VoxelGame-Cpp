#include "VertexBuffer.h"

#include <cassert>
#include <cstring>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"

void createSendBuffer(const rk::VulkanApp* app, u64 size, const void* data, VkBuffer* buffer, VkDeviceMemory* memory, u32 type);

void rk::VertexBuffer::create(const VulkanApp* app, u64 verticesSize,
    const void* verticesData, u64 indicesSize,
    const void* indicesData, i32 indexType) {
    createSendBuffer(app, verticesSize, verticesData, &m_buffer, &m_memory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    createSendBuffer(app, indicesSize, indicesData, &m_indexBuffer, &m_indexMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    m_indexBufferType = indexType;
}

void rk::VertexBuffer::destroy(VkDevice device) const {
    vkDestroyBuffer(device, m_buffer, nullptr);
    vkFreeMemory(device, m_memory, nullptr);

    vkDestroyBuffer(device, m_indexBuffer, nullptr);
    vkFreeMemory(device, m_indexMemory, nullptr);
}

void rk::VertexBuffer::bind(VkCommandBuffer command) const {
    u64 offset = 0;

    vkCmdBindVertexBuffers(command, 0, 1, &m_buffer, &offset);
    vkCmdBindIndexBuffer(command, m_indexBuffer, 0, (VkIndexType)m_indexBufferType);
}

void createSendBuffer(const rk::VulkanApp* app, u64 size, const void* data, VkBuffer* buffer, VkDeviceMemory* memory, u32 type) {
    auto logicalDevice = app->logicalDevice.get();

    // create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    rk::utl::createBuffer(app, size, &stagingBuffer, &stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // upload data
    void* data2;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, size, 0, &data2);
    std::memcpy(data2, data, size);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    // create buffer in vram
    rk::utl::createBuffer(app, size, buffer, memory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | type,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    rk::utl::copyBuffer(app, stagingBuffer, *buffer, size);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}