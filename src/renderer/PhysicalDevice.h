#pragma once

#include <vulkan/vulkan.h>

#include <vector>


namespace rk {
    // fwd
    class VulkanApp;
    class SwapChain;
    
    class PhysicalDevice {
    public:
        void create(const VulkanApp* app);

        VkPhysicalDevice get() const { return m_physicalDevice; }
        const std::vector<VkQueueFamilyProperties>* getQueueFamiliesCache() const { return &m_queues; }

    private:
        void saveQueuesCache();

        VkPhysicalDevice m_physicalDevice = nullptr;
        std::vector<VkQueueFamilyProperties> m_queues;
    };
}

