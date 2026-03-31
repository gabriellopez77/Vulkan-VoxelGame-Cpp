#pragma once

#include <optional>
#include <array>

#include "Defs.h"
#include "PhysicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "LogicalDevice.h"


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

        static void init(GLFWwindow* window);
        static VulkanApp* get() { return m_instance; }
        void resize();

        void clear();

        void beginFrame();
        void endFrame();

        VkInstance getVkInstance() const { return m_vkInstance; }
        VkCommandBuffer getCurrentCommandBuffer() const { return m_commandBuffers[m_currentFrame]; }
        u32 getCurrentFrame() const { return m_currentFrame; }
        VkDescriptorPool getDescriptorPool() const { return m_descriptorPool; }
        VkCommandPool getTransferCommandPool() const { return m_transferCommandPool; }

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device = nullptr,
        const std::vector<VkQueueFamilyProperties>* queues = nullptr) const;

        GLFWwindow* window = nullptr;
        PhysicalDevice physicalDevice;
        SwapChain swapChain;
        LogicalDevice logicalDevice;
        RenderPass renderPass;

    private:
        static VulkanApp* m_instance;

        void createInstance();
        void createCommandPool();
        void createCommandBuffers();
        void createSyncObjects();
        void createDescriptorPool();

        u32 m_lastFrame = 0;
        u32 m_currentFrame = 0;
        u32 m_currentImage = 0;

        // commands
        VkCommandPool m_transferCommandPool = nullptr;
        VkCommandPool m_graphicsCommandPool = nullptr;
        std::array<VkCommandBuffer, utl::FRAMES_COUNT> m_commandBuffers{};

        VkDescriptorPool m_descriptorPool = nullptr;

        // vk instance
        VkInstance m_vkInstance = nullptr;

        // fence
        std::array<VkSemaphore, utl::FRAMES_COUNT> m_imageAvailableSemaphore{};
        std::array<VkSemaphore, utl::FRAMES_COUNT> m_renderFinishedSemaphore{};
        std::array<VkFence, utl::FRAMES_COUNT> m_inFlightFence{};
        std::array<VkFence, utl::FRAMES_COUNT> m_imagesInFlight{};

        VkViewport m_viewport = { .maxDepth = 1.f };
        VkRect2D m_scissor{};
    };
}
