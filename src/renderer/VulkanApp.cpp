#include "VulkanApp.h"

#include <GLFW/glfw3.h>

#include <cassert>

#include "Utils.h"
#include "PipelineSettings.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

constexpr Vertex vertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

constexpr uint16_t indices[] = {
    0, 1, 2, 2, 3, 0
};

void rk::VulkanApp::init(GLFWwindow* window) {
    this->window = window;

    createInstance();

    swapChain.createSurface(this, window);
    physicalDevice.create(this); // pick physical device
    logicalDevice.create(this); // create logical device
    swapChain.create(this); // create swapChain
    renderPass.create(this); // create renderPass
    swapChain.createFramebuffers(this);
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
    createDescriptorPool();


    ubo.create(this, 256);
    descriptorSetLayout.create(this, 0, VK_SHADER_STAGE_VERTEX_BIT);
    descriptorSetLayout.setUbo(this, ubo, 0);
    pushConstants.create(sizeof(int), VK_SHADER_STAGE_VERTEX_BIT);


    PipelineSettings pipelineSettings;
    pipelineSettings.setShaders(SHADERS_FOLDER"/vertex1.spv", SHADERS_FOLDER"/fragment1.spv");
    pipelineSettings.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    pipelineSettings.addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

    pipelineSettings.addBindings(0, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(Vertex));
    pipelineSettings.addAttributes(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
    pipelineSettings.addAttributes(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));

    pipelineSettings.addPushConstants(pushConstants);
    pipelineSettings.addDescriptorSets(descriptorSetLayout);

    graphicsPipeline1.create(this, pipelineSettings); // create graphics pipeline 1

    vertexBuffer.create(this, sizeof(vertices), vertices, sizeof(indices), indices, VK_INDEX_TYPE_UINT16);


    m_viewport.width = swapChain.getWidth();
    m_viewport.height = swapChain.getHeight();

    m_scissor.extent = swapChain.getSize();
}

void rk::VulkanApp::clear() {
    vkDeviceWaitIdle(logicalDevice.get());

    swapChain.clear(this);
}

void rk::VulkanApp::resize() {
    swapChain.recreate(this, m_inFlightFence[m_lastFrame]);

    m_viewport.width = swapChain.getWidth();
    m_viewport.height = swapChain.getHeight();

    m_scissor.extent = swapChain.getSize();
}

void rk::VulkanApp::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // get required extensions
    auto requiredExtensions = utl::getRequiredInstanceExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // validation layer
    if (utl::VALIDATION_LAYERS_ENABLED) {
        createInfo.enabledLayerCount = std::size(utl::validateLayerNames);
        createInfo.ppEnabledLayerNames = utl::validateLayerNames;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        assert(false && "Failed to create Vulkan instance!");
}

rk::VulkanApp::QueueFamilyIndices rk::VulkanApp::findQueueFamilies(VkPhysicalDevice device, const std::vector<VkQueueFamilyProperties>* queues) const {
    auto* queuesFamilies = queues == nullptr ? physicalDevice.getQueueFamiliesCache() : queues;

    // try to find a queue family that supports graphics
    QueueFamilyIndices indices;
    for (int i = 0; i < queuesFamilies->size(); i++) {
        if ((*queuesFamilies)[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if ((*queuesFamilies)[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transferFamily = i;
        }

        // check if the queue family supports presentation to our window surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, swapChain.getSurface(), &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
            break;
    }

    return indices;
}

void rk::VulkanApp::createCommandPool() {
    auto queueFamilyIndices = findQueueFamilies(physicalDevice.get());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice.get(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        assert(false && "failed to create command pool!");
}

void rk::VulkanApp::createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = utl::FRAMES_COUNT;


    if (vkAllocateCommandBuffers(logicalDevice.get(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
        assert(false && "failed to allocate command buffers!");
}

void rk::VulkanApp::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        if (vkCreateSemaphore(logicalDevice.get(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice.get(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice.get(), &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS) {
            assert(false && "failed to create semaphores!");
        }
    }
}

void rk::VulkanApp::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = utl::FRAMES_COUNT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = utl::FRAMES_COUNT;

    if (vkCreateDescriptorPool(logicalDevice.get(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
        assert(false && "failed to create descriptor pool!");
}
glm::mat4 model1(1.f);
glm::mat4 model2(1.f);
void rk::VulkanApp::drawFrame() {
    auto commandBuffer = m_commandBuffers[m_currentFrame];

    vertexBuffer.bind(commandBuffer);
    graphicsPipeline1.bind(commandBuffer);
    descriptorSetLayout.bind(commandBuffer, graphicsPipeline1.getLayout(), m_currentFrame);

    float time = (float)glfwGetTime();

    model1 = glm::translate(glm::mat4(1.f), glm::vec3(swapChain.getWidth() / 2.f, swapChain.getHeight() / 2.f, 0.f));
    model1 = glm::rotate(model1,  glm::radians(time) * 20.f, glm::vec3(0.f, 0.f, 1.f));
    model1 = glm::scale(model1, glm::vec3(100.f));

    model2 = glm::translate(glm::mat4(1.f), glm::vec3(swapChain.getWidth() / 2.f, 0.f, 0.f));
    model2 = glm::rotate(model2,  glm::radians(time) * 20.f, glm::vec3(0.f, 0.f, 1.f));
    model2 = glm::scale(model2, glm::vec3(300.f));
    glm::mat4 matrices1[] = {
        glm::ortho(0.f, (float)swapChain.getWidth(), 0.f, (float)swapChain.getHeight(), -10.f, 10.f),
        model1,
        glm::ortho(0.f, (float)swapChain.getWidth(), 0.f, (float)swapChain.getHeight(), -10.f, 10.f),
        model2,
    };

    ubo.update(m_currentFrame, 0, sizeof(matrices1), matrices1);

    int index0 = 0;
    int index1 = 1;
    pushConstants.bind(commandBuffer, graphicsPipeline1.getLayout(), sizeof(int), &index0);
    vkCmdDrawIndexed(commandBuffer, std::size(indices), 1, 0, 0, 0);

    pushConstants.bind(commandBuffer, graphicsPipeline1.getLayout(), sizeof(int), &index1);
    vkCmdDrawIndexed(commandBuffer, std::size(indices), 1, 0, 0, 0);
}

void rk::VulkanApp::beginFrame() {
    // saves a copy in stack because it is faster
    auto currentFrame = m_currentFrame;

    // wait for previous frame and reset fence state
    vkWaitForFences(logicalDevice.get(), 1, &m_inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(logicalDevice.get(), 1, &m_inFlightFence[currentFrame]);

    u32 imageIndex = swapChain.getOneImage(logicalDevice.get(), m_imageAvailableSemaphore[currentFrame]);


    constexpr VkClearValue clearColor = { .color = { .float32 = { 0.f, 0.f, 0.f, 1.f } } };

    auto commandBuffer = m_commandBuffers[currentFrame];

    // rest command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        assert(false && "failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.get();
    renderPassInfo.framebuffer = swapChain.getFramebuffer(imageIndex);
    renderPassInfo.renderArea.extent = swapChain.getSize();
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


    // set dynamic states
    vkCmdSetViewport(commandBuffer, 0, 1, &m_viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &m_scissor);
}

void rk::VulkanApp::endFrame() {
    // saves a copy in stack because it is faster
    auto currentFrame = m_currentFrame;
    auto commandBuffer = m_commandBuffers[currentFrame];
    u32 imageIndex = swapChain.getImageIndex();

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        assert(false && "failed to record command buffer!");


    const auto signalSemaphores = m_renderFinishedSemaphore[currentFrame];
    constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_imageAvailableSemaphore[currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphores;

    if (vkQueueSubmit(logicalDevice.getGraphicsQueue(), 1, &submitInfo, m_inFlightFence[currentFrame]) != VK_SUCCESS)
        assert(false && "failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain.getSwapChain();
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(logicalDevice.getPresentQueue(), &presentInfo);

    //advance to the next frame
    currentFrame = (currentFrame + 1) % utl::FRAMES_COUNT;
    m_lastFrame = m_currentFrame;
    m_currentFrame = currentFrame;
}