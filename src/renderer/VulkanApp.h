#pragma once

#include <optional>
#include <array>

#include "Defs.h"
#include "PhysicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "VertexBuffer.h"
#include "PushConstants.h"
#include "DescriptorSetLayout.h"
#include "Ubo.h"
#include "Utils.h"


// fwd
struct GLFWwindow;

namespace rk {
    class VulkanApp {
    public:
        struct QueueFamilyIndices {
            std::optional<u32> graphicsFamily;
            std::optional<u32> presentFamily;
            std::optional<u32> transferFamily;

            bool isComplete() const {
                return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
            }
        };

        void init(GLFWwindow* window);
        void clear();
        void resize();
        void drawFrame();

        void beginFrame();
        void endFrame();

        u32 getCurrentImage() const { return m_currentImage; }
        VkDescriptorPool getDescriptorPool() const { return m_descriptorPool; }
        VkCommandPool getCommandPool() const { return m_commandPool; }
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const std::vector<VkQueueFamilyProperties>* queues = nullptr) const;

        GLFWwindow* window = nullptr;
        PhysicalDevice physicalDevice;
        SwapChain swapChain;
        RenderPass renderPass;
        GraphicsPipeline graphicsPipeline1;
        LogicalDevice logicalDevice;


        VkInstance instance = nullptr;

        PushConstants pushConstants;
        VertexBuffer vertexBuffer;
        Ubo ubo;
        DescriptorSetLayout descriptorSetLayout;
        
    private:
        void createInstance();
        void createCommandPool();
        void createCommandBuffers();
        void createSyncObjects();
        void createDescriptorPool();

        u32 m_lastFrame = 0;
        u32 m_currentFrame = 0;
        u32 m_currentImage = 0;

        // commands
        VkCommandPool m_commandPool = nullptr;
        std::array<VkCommandBuffer, utl::FRAMES_COUNT> m_commandBuffers{};

        VkDescriptorPool m_descriptorPool = nullptr;

        // fence
        std::array<VkSemaphore, utl::FRAMES_COUNT> m_imageAvailableSemaphore{};
        std::array<VkSemaphore, utl::FRAMES_COUNT> m_renderFinishedSemaphore{};
        std::array<VkFence, utl::FRAMES_COUNT> m_inFlightFence{};

        VkViewport m_viewport = { .maxDepth = 1.f };
        VkRect2D m_scissor{};
    };
}
