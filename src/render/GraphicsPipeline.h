#pragma once


#include "core/VulkanFwd.h"
#include "VulkanEnums.h"


namespace rk {
    class SwapChain;
    class PipelineSettings;

    class GraphicsPipeline {
    public:
        void create(const PipelineSettings& settings);
        void bind(VkCommandBuffer command) const;

        void bindPushConstant(VkCommandBuffer command, u64 size, const void* data) const;

        VkPipeline get() const { return m_graphicsPipeline; }
        VkPipelineLayout getLayout() const { return m_pipelineLayout; }

    private:
        bool m_usePushConstant = false;
        ShaderStage m_pushConstantShaderStage{};

        VkPipelineLayout m_pipelineLayout = nullptr;
        VkPipeline m_graphicsPipeline = nullptr;
    };
}
