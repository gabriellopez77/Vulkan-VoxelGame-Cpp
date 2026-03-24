#pragma once

#include <optional>
#include <vector>

#include "VulkanFwd.h"
#include "../defs.h"
#include "PhysicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "VertexBuffer.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


// fwd
struct GLFWwindow;

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributes;

        attributes[0].location = 0;
        attributes[0].binding = 0;
        attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[0].offset = offsetof(Vertex, pos);

        attributes[1].location = 1;
        attributes[1].binding = 0;
        attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset = offsetof(Vertex, color);

        return attributes;
    }
};

namespace render {
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
        PhysicalDevice physicalDevice;
        SwapChain swapChain;
        RenderPass renderPass;
        GraphicsPipeline graphicsPipeline1;
        //GraphicsPipeline graphicsPipeline2;
        LogicalDevice logicalDevice;

        VkInstance instance = nullptr;

        render::VertexBuffer vertexBuffer;
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
}
