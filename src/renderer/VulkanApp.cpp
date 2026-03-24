#include "VulkanApp.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <fstream>
#include <cassert>

#include "Utils.h"
#include "PipelineSettings.h"


const std::vector<Vertex> vertices = {
    {{-0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
};

void render::VulkanApp::init(GLFWwindow* window) {
    this->window = window;

    createInstance();

    swapChain.createSurface(this, window);
    physicalDevice.create(this); // pick physical device
    logicalDevice.create(this); // create logical device
    swapChain.create(this); // create swapChain
    swapChain.createImageViews(this);
    renderPass.create(this); // create renderPass

    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    binding.stride = sizeof(Vertex);

    // dynamic states
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    PipelineSettings pipelineSettings;
    pipelineSettings.vertPath = SHADERS_FOLDER"/vertex1.spv";
    pipelineSettings.fragPath = SHADERS_FOLDER"/fragment1.spv";
    pipelineSettings.attributesCount = attributeDescriptions.size();
    pipelineSettings.attributes = attributeDescriptions.data();
    pipelineSettings.bindingsCount = 1;
    pipelineSettings.bindings = &binding;
    pipelineSettings.dynamicStatesCount = 2;
    pipelineSettings.dynamicStates = dynamicStates;

    graphicsPipeline1.create(this, pipelineSettings); // create graphics pipeline 1
    swapChain.createFramebuffers(this); // create framebuffers

    createCommandPool();
    vertexBuffer.create(this, sizeof(vertices[0]) * vertices.size(), vertices.data());
    createCommandBuffers();
    createSyncObjects();
}

void render::VulkanApp::clear() {
    vkDeviceWaitIdle(logicalDevice.get());

    swapChain.clear(this);
}

void render::VulkanApp::resize() {
    swapChain.needRecreate();
}

void render::VulkanApp::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;


    // get required extensions
    auto requiredExtensions = utils::getRequiredInstanceExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();


    // validation layer
    if (VALIDATION_LAYERS_ENABLED) {
        createInfo.enabledLayerCount = std::size(utils::validateLayerNames);
        createInfo.ppEnabledLayerNames = utils::validateLayerNames;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        assert(false && "Failed to create Vulkan instance!");
    }
}

render::VulkanApp::QueueFamilyIndices render::VulkanApp::findQueueFamilies(VkPhysicalDevice device) const {
    // get queue family properties count
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // get queue family properties
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


    // try to find a queue family that supports graphics
    QueueFamilyIndices indices;
    for (int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
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

void render::VulkanApp::createCommandPool() {
    auto queueFamilyIndices = findQueueFamilies(physicalDevice.get());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice.get(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        assert(false && "failed to create command pool!");
}

void render::VulkanApp::createCommandBuffers() {
    m_commandBuffers.resize(swapChain.getImagesCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = swapChain.getImagesCount();

    if (vkAllocateCommandBuffers(logicalDevice.get(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
        assert(false && "failed to allocate command buffers!");
}

void render::VulkanApp::createSyncObjects() {
    m_imageAvailableSemaphore.resize(swapChain.getImagesCount());
    m_renderFinishedSemaphore.resize(swapChain.getImagesCount());
    m_inFlightFence.resize(swapChain.getImagesCount());

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < swapChain.getImagesCount(); i++) {
        if (vkCreateSemaphore(logicalDevice.get(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice.get(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice.get(), &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS) {
            assert(false && "failed to create semaphores!");
        }
    }
}

void render::VulkanApp::recordCommandBuffer(u32 imageIndex) const {
    constexpr VkClearValue clearColor = { .color = { .float32 = { 0.f, 0.f, 0.f, 1.f } } };

    auto commandBuffer = m_commandBuffers[m_currentFrame];

    // rest command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        assert(false && "failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.get();
    renderPassInfo.framebuffer = swapChain.getFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain.getSize();
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // set dynamic states
    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = swapChain.getWidth();
    viewport.height = swapChain.getHeight();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain.getSize();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    graphicsPipeline1.bind(commandBuffer);
    vertexBuffer.bind(commandBuffer);

    // bind pipeline 2
    vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        assert(false && "failed to record command buffer!");
}

void render::VulkanApp::drawFrame() {
    // wait for previous frame
    vkWaitForFences(logicalDevice.get(), 1, &m_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);

    // get one image from swap chain
    u32 imageIndex = swapChain.getOneImage(this, m_imageAvailableSemaphore[m_currentFrame]);

    // set default fences state
    vkResetFences(logicalDevice.get(), 1, &m_inFlightFence[m_currentFrame]);

    // record draw infos in commandBuffer
    recordCommandBuffer(imageIndex);


    const VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore[m_currentFrame] };
    const VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore[m_currentFrame] };
    const VkSwapchainKHR swapChains[] = { swapChain.getSwapChain() };
    const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(logicalDevice.getGraphicsQueue(), 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS)
        assert(false && "failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    auto result = vkQueuePresentKHR(logicalDevice.getPresentQueue(), &presentInfo);

    // recreate swap chain if necessary
    swapChain.tryRecreate(this, result, &m_inFlightFence[m_currentFrame]);

    //advance to the next frame
    m_currentFrame = (m_currentFrame + 1) % swapChain.getImagesCount();
}