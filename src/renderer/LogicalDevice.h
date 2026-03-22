#pragma once


class PhysicalDevice;
class VulkanApp;
typedef struct VkQueue_T *VkQueue;
typedef struct VkDevice_T *VkDevice;

class LogicalDevice {
public:
    void init(const VulkanApp* app);

    VkDevice get() const { return m_logicalDevice; }
    VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
    VkQueue getPresentQueue() const { return m_presentQueue; }

private:
    VkDevice m_logicalDevice = nullptr;

    // queues
    VkQueue m_graphicsQueue = nullptr;
    VkQueue m_presentQueue = nullptr;
};