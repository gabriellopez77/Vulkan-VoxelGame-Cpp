#include "VertexBuffer.h"

#include <cassert>

#include <vulkan/vulkan.h>

#include "core/VulkanApp.h"
#include "VulkanEnums.h"


rk::VertexBuffer& rk::VertexBuffer::createVertices(u32 size, const void* data, VertexBufferType updateType) {
    m_verticesInfo = {};

    m_verticesInfo.updateType = updateType;
    m_verticesInfo.size = size;
    createSendBuffer(data, m_verticesInfo, BufferUsage::VERTEX_BUFFER);

    return *this;
}

rk::VertexBuffer& rk::VertexBuffer::createIndices(u32 size, const u32* data, VertexBufferType updateType) {
    m_indicesInfo = {};

    m_indicesInfo.updateType = updateType;
    m_indicesInfo.size = size;
    createSendBuffer(data, m_indicesInfo, BufferUsage::INDEX_BUFFER);

    return *this;
}

void rk::VertexBuffer::destroy() const {
    VkDevice device = vulkanApp::getLogicalDevice();

    if (m_verticesInfo.updateType == VertexBufferType::RAM)
        vkUnmapMemory(vulkanApp::getLogicalDevice(), m_verticesInfo.memory);

    vkDestroyBuffer(device, m_verticesInfo.buffer, nullptr);
    vkFreeMemory(device, m_verticesInfo.memory, nullptr);


    if (m_indicesInfo.updateType == VertexBufferType::RAM)
        vkUnmapMemory(vulkanApp::getLogicalDevice(), m_indicesInfo.memory);

    vkDestroyBuffer(device, m_indicesInfo.buffer, nullptr);
    vkFreeMemory(device, m_indicesInfo.memory, nullptr);
}

void rk::VertexBuffer::bind(VkCommandBuffer command, u32 binding) const {
    const u64 offset = 0;

    vkCmdBindVertexBuffers(command, binding, 1, &m_verticesInfo.buffer, &offset);

    if (m_indicesInfo.buffer != nullptr)
        vkCmdBindIndexBuffer(command, m_indicesInfo.buffer, 0, VK_INDEX_TYPE_UINT32);
}

void rk::VertexBuffer::update(BufferInfo& bufferInfo, u32 size, const void* data) {
    assert(size <= bufferInfo.size);

    if (bufferInfo.updateType == VertexBufferType::RAM) {
        std::memcpy(bufferInfo.mappedMemory, data, size);
    }
    else assert(false && "buffer can not be updated, because it is ONE_TIME");
}

void rk::VertexBuffer::createSendBuffer(const void* data, BufferInfo& bufferInfo, BufferUsage usage) {
    auto logicalDevice = rk::vulkanApp::getLogicalDevice();

    // upload data to vram
    if (bufferInfo.updateType == VertexBufferType::ONE_TIME) {
        assert(data != nullptr);

        // create staging buffer
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;
        utl::createBuffer(bufferInfo.size, stagingBuffer, stagingMemory, BufferUsage::TRANSFER_SRC,
            MemoryType::HOST_VISIBLE | MemoryType::HOST_COHERENT);

        // upload data
        utl::copyDataToStagingBuffer(bufferInfo.size, stagingMemory, data);

        // create buffer in VRAM
        utl::createBuffer(bufferInfo.size, bufferInfo.buffer, bufferInfo.memory,
            BufferUsage::TRANSFER_DST | usage, MemoryType::DEVICE_LOCAL);

        // copy staging buffer to vram buffer
        utl::copyBuffer(stagingBuffer, bufferInfo.buffer, bufferInfo.size);

        vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice, stagingMemory, nullptr);
    }
    else {
        utl::createBuffer(bufferInfo.size, bufferInfo.buffer, bufferInfo.memory, usage,
            MemoryType::HOST_VISIBLE | MemoryType::HOST_COHERENT);

        vkMapMemory(logicalDevice, bufferInfo.memory, 0, bufferInfo.size, 0, &bufferInfo.mappedMemory);
    }

}