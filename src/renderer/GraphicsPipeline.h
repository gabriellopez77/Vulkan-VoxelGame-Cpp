#pragma once

#include "VulkanFwd.h"


namespace rk {
    //fwd
    class SwapChain;
    class RenderPass;
    class VulkanApp;
    class PipelineSettings;

    class GraphicsPipeline {
    public:
        void create(const PipelineSettings& settings);
        void bind(VkCommandBuffer command) const;

        VkPipeline get() const { return m_graphicsPipeline; }
        VkPipelineLayout getLayout() const { return m_pipelineLayout; }

    private:
        VkPipelineLayout m_pipelineLayout = nullptr;
        VkPipeline m_graphicsPipeline = nullptr;
    };
}
