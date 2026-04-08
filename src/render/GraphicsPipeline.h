#pragma once

#include <vector>
#include <array>

#include "core/Utils.h"


namespace rk {
    class PipelineSettings;
    class DescriptorSet;

    class GraphicsPipeline {
    public:
        void create(const PipelineSettings& settings);
        void bind(VkCommandBuffer command);

        void bindPushConstant(VkCommandBuffer command, u64 size, const void* data) const;

        VkPipeline get() const { return m_graphicsPipeline; }
        VkPipelineLayout getLayout() const { return m_pipelineLayout; }

    private:
        std::array<std::vector<VkDescriptorSet>, 3> m_vkDescriptorSets;
        std::vector<DescriptorSet*> m_descriptorSets;
        std::vector<u32> m_dynamicUboOffsets;

        bool m_usePushConstant = false;
        ShaderStage m_pushConstantShaderStage{};

        VkPipelineLayout m_pipelineLayout = nullptr;
        VkPipeline m_graphicsPipeline = nullptr;
    };
}
