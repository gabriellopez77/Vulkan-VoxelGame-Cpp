#include "Utils.h"

#include <cassert>
#include <fstream>

#include <vulkan/vulkan.h>

#include <glfw/glfw3.h>

#include "VulkanApp.h"
#include "VulkanEnums.h"


namespace rk::utl {
    std::vector<const char*> getRequiredInstanceExtensions() {
        std::vector<const char*> extensions;

        u32 glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (int i = 0; i < glfwExtensionCount; i++)
            extensions.push_back(glfwExtensions[i]);

        if (VALIDATION_LAYERS_ENABLED)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    std::vector<char> readShaderFile(const char* filePath) {
        // open the file at the end to get the file size
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            assert(false && "failed to open shader file!");
        }

        // get file size and allocate a buffer for it
        u64 fileSize = file.tellg();
        std::vector<char> buffer(fileSize);

        // set the file position to the beginning and read the file into the buffer
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    u32 findMemoryType(u32 type, MemoryType memoryType) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(VulkanApp::get()->physicalDevice.get(), &memProperties);

        for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
            if (type & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & (u32)memoryType) == (u32)memoryType) {
                return i;
            }
        }

        assert(false && "failed to find suitable memory type!");
    }

    void copyDataToStagingBuffer(u64 size, VkDeviceMemory memoryType, const void* data) {
        auto logicalDevice = VulkanApp::get()->logicalDevice.get();

        void* dataPtr;
        vkMapMemory(logicalDevice, memoryType, 0, size, 0, &dataPtr);
        memcpy(dataPtr, data, size);
        vkUnmapMemory(logicalDevice, memoryType);
    }

    void createBuffer(u64 size, VkBuffer& buffer, VkDeviceMemory& memory, BufferUsage usage, MemoryType memoryType) {
        auto logicalDevice = VulkanApp::get()->logicalDevice.get();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = (u32)usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // create buffer
        if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
            assert(false && "failed to create buffer!");

        // get memory requirements
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(logicalDevice, buffer, &requirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = requirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, memoryType);

        // allocate buffer memory
        if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS)
            assert(false && "failed to allocate buffer memory!");

        // bind memory to buffer
        vkBindBufferMemory(logicalDevice, buffer, memory, 0);
    }

    void copyBuffer(VkBuffer src, VkBuffer dst, u64 size) {
        auto logicalDevice = VulkanApp::get()->logicalDevice;

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = VulkanApp::get()->getTransferCommandPool();

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(logicalDevice.get(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(logicalDevice.getTransferQueue(), 1, &submitInfo, nullptr);
        vkQueueWaitIdle(logicalDevice.getTransferQueue());

        vkFreeCommandBuffers(logicalDevice.get(), VulkanApp::get()->getTransferCommandPool(), 1, &commandBuffer);
    }

    void createImage(u32 width, u32 height) {

    }
}
