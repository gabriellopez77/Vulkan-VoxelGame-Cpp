#include "VertexBuffer.h"

#include <cassert>
#include <cstring>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"


namespace render {
    void VertexBuffer::create(const VulkanApp* app, u64 size, const void* data) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // create buffer
        if (vkCreateBuffer(app->logicalDevice.get(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
            assert(false && "failed to create buffer!");

        // get memory requirements
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(app->logicalDevice.get(), m_buffer, &requirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = requirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(app, requirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // allocate buffer memory
        if (vkAllocateMemory(app->logicalDevice.get(), &allocInfo, nullptr, &m_memory) != VK_SUCCESS)
            assert(false && "failed to allocate buffer memory!");

        // bind memory to buffer
        vkBindBufferMemory(app->logicalDevice.get(), m_buffer, m_memory, 0);

        // set buffer data
        void* data2;
        vkMapMemory(app->logicalDevice.get(), m_memory, 0, bufferInfo.size, 0, &data2);
        std::memcpy(data2, data, bufferInfo.size);
        vkUnmapMemory(app->logicalDevice.get(), m_memory);
    }

    void VertexBuffer::destroy(VkDevice device) const {
        vkDestroyBuffer(device, m_buffer, nullptr);
        vkFreeMemory(device, m_memory, nullptr);
    }

    void VertexBuffer::bind(VkCommandBuffer command) const {
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command, 0, 1, &m_buffer, offsets);
    }

    u32 VertexBuffer::findMemoryType(const VulkanApp* app, u32 type, u32 properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(app->physicalDevice.get(), &memProperties);

        for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
            if (type & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        assert(false && "failed to find suitable memory type!");
    }
}
