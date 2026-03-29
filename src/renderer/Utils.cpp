#include "Utils.h"

#include <cassert>
#include <fstream>

#include <vulkan/vulkan.h>

#include <glfw/glfw3.h>

#include "VulkanApp.h"


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

    u32 findMemoryType(const VulkanApp* app, u32 type, u32 properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(app->physicalDevice.get(), &memProperties);

        for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
            if (type & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        assert(false && "failed to find suitable memory type!");
    }

    void createBuffer(const VulkanApp* app, u64 size, VkBuffer* buffer, VkDeviceMemory* memory, u32 usage, u32 properties) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // create buffer
        if (vkCreateBuffer(app->logicalDevice.get(), &bufferInfo, nullptr, buffer) != VK_SUCCESS)
            assert(false && "failed to create buffer!");

        // get memory requirements
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(app->logicalDevice.get(), *buffer, &requirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = requirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(app, requirements.memoryTypeBits, properties);

        // allocate buffer memory
        if (vkAllocateMemory(app->logicalDevice.get(), &allocInfo, nullptr, memory) != VK_SUCCESS)
            assert(false && "failed to allocate buffer memory!");

        // bind memory to buffer
        vkBindBufferMemory(app->logicalDevice.get(), *buffer, *memory, 0);
    }

    void copyBuffer(const VulkanApp* app, VkBuffer src, VkBuffer dst, u64 size) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = app->getCommandPool();

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(app->logicalDevice.get(), &allocInfo, &commandBuffer);

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

        vkQueueSubmit(app->logicalDevice.getTransferQueue(), 1, &submitInfo, nullptr);
        vkQueueWaitIdle(app->logicalDevice.getTransferQueue());

        vkFreeCommandBuffers(app->logicalDevice.get(), app->getCommandPool(), 1, &commandBuffer);
    }
}
