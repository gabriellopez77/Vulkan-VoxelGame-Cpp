#pragma once

#include <optional>

#include "VulkanFwd.h"
#include "Defs.h"


// fwd
struct VkPhysicalDeviceProperties;

namespace rk::vulkanApp {
    struct QueueFamilyIndices {
        std::optional<u32> graphics;
        std::optional<u32> present;
        std::optional<u32> transfer;

        bool isComplete() const {
            return graphics.has_value() && present.has_value() && transfer.has_value();
        }
    };

    extern void init();

    extern void resize();

    extern void clear();

    extern void beginFrame();
    extern void endFrame();

    extern u32 getImageIndex();

    extern VkDevice getLogicalDevice();
    extern VkQueue getGraphicsQueue();
    extern VkQueue getPresentQueue();
    extern VkQueue getTransferQueue();

    extern VkRenderPass getRenderPass();

    extern const VkPhysicalDeviceProperties& getProperties();
    extern VkPhysicalDevice getPhysicalDevice();
    extern VkInstance getVkInstance();

    extern VkCommandBuffer getCurrentCommandBuffer();
    extern VkDescriptorPool getDescriptorPool();
    extern VkCommandPool getTransferCommandPool();

    extern QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device = nullptr);
}
