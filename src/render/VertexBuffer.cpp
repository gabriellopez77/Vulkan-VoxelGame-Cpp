#include "VertexBuffer.h"

#include <cassert>

#include <vulkan/vulkan.h>

#include "core/VulkanApp.h"
#include "VulkanEnums.h"


rk::VertexBuffer& rk::VertexBuffer::createVertices(u32 size, const void* data, UpdateType updateType) {
    m_verticesUpdateType = updateType;
    m_verticesBufferSize = size;

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        m_verticesInfo[i] = {};

        createOrSendBuffer(size, data, m_verticesInfo[i], updateType, BufferUsage::VERTEX_BUFFER);

        // if buffer is one time then do not make buffer copies
        if (updateType == UpdateType::ONE_TIME)
            break;
    }

    return *this;
}

rk::VertexBuffer& rk::VertexBuffer::createIndices(u32 size, const u32* data, UpdateType updateType) {
    m_indicesUpdateType = updateType;
    m_indicesBufferSize = size;

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        m_indicesInfo[i] = {};

        createOrSendBuffer(size, data, m_indicesInfo[i], updateType, BufferUsage::INDEX_BUFFER);

        // if buffer is one time then do not make buffer copies
        if (updateType == UpdateType::ONE_TIME)
            break;
    }

    return *this;
}

void rk::VertexBuffer::destroy() const {
    VkDevice device = vulkanApp::getLogicalDevice();

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        if (m_verticesUpdateType == UpdateType::OFTEN)
            vkUnmapMemory(vulkanApp::getLogicalDevice(), m_verticesInfo[i].memory);

        vkDestroyBuffer(device, m_verticesInfo[i].buffer, nullptr);
        vkFreeMemory(device, m_verticesInfo[i].memory, nullptr);

        // if buffer is one time then do not make buffer copies
        if (m_verticesUpdateType == UpdateType::ONE_TIME)
            break;
    }

    // we dont have indices buffer to free
    if (m_indicesBufferSize == 0)
        return;

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        if (m_indicesUpdateType == UpdateType::SOMETIMES) {

        }
        if (m_indicesUpdateType == UpdateType::OFTEN)
            vkUnmapMemory(vulkanApp::getLogicalDevice(), m_indicesInfo[i].memory);

        vkDestroyBuffer(device, m_indicesInfo[i].buffer, nullptr);
        vkFreeMemory(device, m_indicesInfo[i].memory, nullptr);

        // if buffer is one time then do not make buffer copies
        if (m_indicesUpdateType == UpdateType::ONE_TIME)
            break;
    }

}

void rk::VertexBuffer::update(u32 size, const void* data) {
    assert(size <= m_verticesBufferSize);

    update(m_verticesInfo[vulkanApp::getImageIndex()], m_verticesUpdateType, size, data);
}

void rk::VertexBuffer::updateIndices(u32 size, const u32* data) {
    assert(size <= m_indicesBufferSize);

    update(m_indicesInfo[vulkanApp::getImageIndex()], m_indicesUpdateType, size, data);
}

void rk::VertexBuffer::bind(VkCommandBuffer command, u32 binding) const {
    const u64 offset = 0;
    const u32 imageIndex = vulkanApp::getImageIndex();

    const u32 verticesIndex = m_verticesUpdateType == UpdateType::ONE_TIME ? 0 : imageIndex;

    vkCmdBindVertexBuffers(command, binding, 1, &m_verticesInfo[verticesIndex].buffer, &offset);

    if (m_indicesBufferSize != 0) {
        const u32 indicesIndex = m_indicesUpdateType == UpdateType::ONE_TIME ? 0 : imageIndex;

        vkCmdBindIndexBuffer(command, m_indicesInfo[indicesIndex].buffer, 0, VK_INDEX_TYPE_UINT32);
    }
}

void rk::VertexBuffer::update(BufferInfo& bufferInfo, UpdateType updateType, u32 size, const void* data) {
    if (updateType == UpdateType::OFTEN) {
        std::memcpy(bufferInfo.mappedMemory, data, size);
    }
    else if (updateType == UpdateType::SOMETIMES) {
        // upload data to staging buffer
        std::memcpy(bufferInfo.mappedMemory, data, size);

        // copy staging buffer to vram buffer
        utl::copyBuffer(bufferInfo.stagingBuffer, bufferInfo.buffer, size);
    }
    else assert(false && "buffer can not be updated, because it is ONE_TIME");
}

void rk::VertexBuffer::createOrSendBuffer(u32 size, const void* data, BufferInfo& bufferInfo, UpdateType updateType, BufferUsage usage) {
    auto logicalDevice = rk::vulkanApp::getLogicalDevice();

    // upload data to vram
    if (updateType == UpdateType::ONE_TIME) {
        assert(data != nullptr);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        // create staging buffer
        utl::createBuffer(size, stagingBuffer, stagingMemory, BufferUsage::TRANSFER_SRC,
            MemoryType::HOST_VISIBLE | MemoryType::HOST_COHERENT);

        // upload data
        utl::copyDataToStagingBuffer(size, stagingMemory, data, false);

        // create buffer in VRAM
        utl::createBuffer(size, bufferInfo.buffer, bufferInfo.memory,
            BufferUsage::TRANSFER_DST | usage, MemoryType::DEVICE_LOCAL);

        // copy staging buffer to vram buffer
        utl::copyBuffer(stagingBuffer, bufferInfo.buffer, size);

        vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice, stagingMemory, nullptr);
    }
    else if (updateType == UpdateType::SOMETIMES) {
        // create staging buffer
        utl::createBuffer(size, bufferInfo.stagingBuffer, bufferInfo.stagingMemory, BufferUsage::TRANSFER_SRC,
            MemoryType::HOST_VISIBLE | MemoryType::HOST_COHERENT);

        // upload data
        bufferInfo.mappedMemory = utl::copyDataToStagingBuffer(size, bufferInfo.stagingMemory, data, false);

        // create buffer in VRAM
        utl::createBuffer(size, bufferInfo.buffer, bufferInfo.memory,
            BufferUsage::TRANSFER_DST | usage, MemoryType::DEVICE_LOCAL);
    }
    else {
        utl::createBuffer(size, bufferInfo.buffer, bufferInfo.memory, usage,
            MemoryType::HOST_VISIBLE | MemoryType::HOST_COHERENT);

        vkMapMemory(logicalDevice, bufferInfo.memory, 0, size, 0, &bufferInfo.mappedMemory);
    }

}