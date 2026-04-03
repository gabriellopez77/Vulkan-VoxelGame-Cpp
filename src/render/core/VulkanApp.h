#pragma once

#include <optional>

#include "SwapChain.h"


// fwd
class Application;

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

        static void init(const Application* application);
        static VulkanApp* get() { return m_instance; }
        static u32 currentFrame() { return m_currentFrame; }

        void resize();

        void clear();

        void beginFrame();
        void endFrame();

        VkDevice getLogicalDevice() const { return m_logicalDevice; }
        VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
        VkQueue getPresentQueue() const { return m_presentQueue; }
        VkQueue getTransferQueue() const { return m_transferQueue; }

        VkRenderPass getRenderPass() const { return m_renderPass; }

        VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
        VkInstance getVkInstance() const { return m_vkInstance; }

        VkCommandBuffer getCurrentCommandBuffer() const { return m_commandBuffers[m_currentFrame]; }
        VkDescriptorPool getDescriptorPool() const { return m_descriptorPool; }
        VkCommandPool getTransferCommandPool() const { return m_transferCommandPool; }

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device = nullptr) const;

        const Application* application = nullptr;

        SwapChain swapChain;

    private:
        static VulkanApp* m_instance;

        void createInstance();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createRenderPass();
        void createCommandPool();
        void createCommandBuffers();
        void createSyncObjects();
        void createDescriptorPool(u32 maxSetsInDescriptors, u32 maxDescriptorCount);

        static u32 m_lastFrame;
        static u32 m_currentFrame;
        static u32 m_currentImage;

        // commands
        VkCommandPool m_transferCommandPool = nullptr;
        VkCommandPool m_graphicsCommandPool = nullptr;
        VkCommandBuffer m_commandBuffers[utl::FRAMES_COUNT] = {};
        VkDescriptorPool m_descriptorPool = nullptr;

        // others
        VkInstance m_vkInstance = nullptr;
        VkPhysicalDevice m_physicalDevice = nullptr;
        VkDevice m_logicalDevice = nullptr;
        VkRenderPass m_renderPass = nullptr;

        // queues
        VkQueue m_graphicsQueue = nullptr;
        VkQueue m_presentQueue = nullptr;
        VkQueue m_transferQueue = nullptr;

        // fence
        VkSemaphore m_imageAvailableSemaphore[utl::FRAMES_COUNT] = {};
        VkSemaphore m_renderFinishedSemaphore[utl::FRAMES_COUNT] = {};
        VkFence m_inFlightFence[utl::FRAMES_COUNT] = {};
        VkFence m_imagesInFlight[utl::FRAMES_COUNT] = {};
    };
}
