#pragma once

#include <vulkan/vulkan.h>

#include <vector>


//fwd
class SwapChain;
class RenderPass;
class VulkanApp;

class GraphicsPipeline {
public:
    void init(const VulkanApp* app, const char* vertPath, const char* fragPath);

    VkPipeline get() const { return m_graphicsPipeline; }
    VkPipelineLayout pipelineLayout = nullptr;

private:
    std::vector<char> readShaderFile(const char* filePath);
    VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);

    VkPipeline m_graphicsPipeline = nullptr;
};