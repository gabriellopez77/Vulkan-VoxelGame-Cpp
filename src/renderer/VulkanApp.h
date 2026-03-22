#pragma once

#include <vulkan/vulkan.h>

#include "../defs.h"

#include <optional>
#include <vector>


struct GLFWwindow;
class PhysicalDevice;
class SwapChain;
class RenderPass;
class GraphicsPipeline;
class LogicalDevice;

class VulkanApp {
public:
    static constexpr bool VALIDATION_LAYERS_ENABLED = true;

    struct QueueFamilyIndices {
        std::optional<u32> graphicsFamily;
        std::optional<u32> presentFamily;

        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    void init(GLFWwindow* window);
    void clear();
    void resize();

    void drawFrame();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

    GLFWwindow* window = nullptr;
    PhysicalDevice* physicalDevice = nullptr;
    SwapChain* swapChain = nullptr;
    RenderPass* renderPass = nullptr;
    GraphicsPipeline* graphicsPipeline1 = nullptr;
    GraphicsPipeline* graphicsPipeline2 = nullptr;
    LogicalDevice* logicalDevice = nullptr;

    VkInstance instance = nullptr;

private:
    void createInstance();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();


    void recordCommandBuffer(u32 imageIndex) const;

    u32 m_currentFrame = 0;

    // commands
    VkCommandPool m_commandPool = nullptr;
    std::vector<VkCommandBuffer> m_commandBuffers = {};

    // fence
    std::vector<VkSemaphore> m_imageAvailableSemaphore;
    std::vector<VkSemaphore> m_renderFinishedSemaphore;
    std::vector<VkFence> m_inFlightFence;
};