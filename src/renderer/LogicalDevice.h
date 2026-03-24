#pragma once

#include "VulkanFwd.h"


namespace render {
    // fwd
    class PhysicalDevice;
    class VulkanApp;

    class LogicalDevice {
    public:
        void create(const VulkanApp* app);

        VkDevice get() const { return m_logicalDevice; }
        VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
        VkQueue getPresentQueue() const { return m_presentQueue; }

    private:
        VkDevice m_logicalDevice = nullptr;

        // queues
        VkQueue m_graphicsQueue = nullptr;
        VkQueue m_presentQueue = nullptr;
    };
}
