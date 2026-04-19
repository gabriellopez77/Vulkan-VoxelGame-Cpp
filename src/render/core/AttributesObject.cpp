#include "AttributesObject.h"

#include <cassert>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"


void rk::AttributesObject::bind(VkCommandBuffer command) {
    const u32 imageIndex = vulkanApp::getImageIndex();

    m_bindBuffers.clear();

    for (const auto& bufferInfo : m_buffersInfo) {
        const u32 index = bufferInfo.updateType == UpdateType::OneTime ? 0 : imageIndex;

        m_bindBuffers.push_back(bufferInfo.buffers[index].buffer);
    }

    // bind al buffer with one bind function
    vkCmdBindVertexBuffers(command, 0, m_bindBuffers.size(), m_bindBuffers.data(), m_buffersOffsets.data());

    // if size == 0 then we do not use index buffer
    if (m_indexBufferInfo.size != 0) {
        const u32 indicesIndex = m_indexBufferInfo.updateType == UpdateType::OneTime ? 0 : imageIndex;

        vkCmdBindIndexBuffer(command, m_indexBufferInfo.buffers[indicesIndex].buffer, 0, VK_INDEX_TYPE_UINT32);
    }
}

void rk::AttributesObject::updateBufferData(BufferInfo& bufferInfo, u32 size, const void* data) {
    assert(size <= bufferInfo.size);

    auto& bufferDataInfo = bufferInfo.buffers[vulkanApp::getImageIndex()];

    if (bufferInfo.updateType == UpdateType::Often) {
        std::memcpy(bufferDataInfo.mappedMemory, data, size);
    }
    else if (bufferInfo.updateType == UpdateType::Sometimes) {
        // upload data to staging buffer
        std::memcpy(bufferDataInfo.mappedMemory, data, size);

        // copy staging buffer to vram buffer
        utl::copyBuffer(bufferDataInfo.stagingBuffer, bufferDataInfo.buffer, size);
    }
    else assert(false && "buffer can not be updated, because it is OneTime");
}

void rk::AttributesObject::createOrSendBuffer(const void* data, i32 index, BufferInfo& vertexBufferInfo, BufferUsage usage) {
    auto logicalDevice = rk::vulkanApp::getLogicalDevice();

    auto& bufferInfo = vertexBufferInfo.buffers[index];

    // upload data to vram
    if (vertexBufferInfo.updateType == UpdateType::OneTime) {
        assert(data != nullptr);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        // create staging buffer
        utl::createBuffer(vertexBufferInfo.size, stagingBuffer, stagingMemory, BufferUsage::TransderSrc,
            MemoryType::HostVisible | MemoryType::HostCoherent);

        // upload data
        utl::copyDataToStagingBuffer(vertexBufferInfo.size, stagingMemory, data, false);

        // create buffer in VRAM
        utl::createBuffer(vertexBufferInfo.size, bufferInfo.buffer, bufferInfo.memory,
            BufferUsage::TransferDst | usage, MemoryType::DeviceLocal);

        // copy staging buffer to vram buffer
        utl::copyBuffer(stagingBuffer, bufferInfo.buffer, vertexBufferInfo.size);

        vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice, stagingMemory, nullptr);
    }
    else if (vertexBufferInfo.updateType == UpdateType::Sometimes) {
        assert(data != nullptr);

        // create staging buffer
        utl::createBuffer(vertexBufferInfo.size, bufferInfo.stagingBuffer, bufferInfo.stagingMemory, BufferUsage::TransderSrc,
            MemoryType::HostVisible | MemoryType::HostCoherent);

        // upload data
        bufferInfo.mappedMemory = utl::copyDataToStagingBuffer(vertexBufferInfo.size, bufferInfo.stagingMemory, data, false);

        // create buffer in VRAM
        utl::createBuffer(vertexBufferInfo.size, bufferInfo.buffer, bufferInfo.memory,
            BufferUsage::TransferDst | usage, MemoryType::DeviceLocal);
    }
    else {
        utl::createBuffer(vertexBufferInfo.size, bufferInfo.buffer, bufferInfo.memory, usage,
            MemoryType::HostVisible | MemoryType::HostCoherent);

        vkMapMemory(logicalDevice, bufferInfo.memory, 0, vertexBufferInfo.size, 0, &bufferInfo.mappedMemory);
    }
}