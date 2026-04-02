#pragma once

#include <vector>

#include "Utils.h"


namespace rk {
    // fwd
    class VulkanApp;
    class Ubo;
    class Texture;
    enum class ShaderStage : i32;

    class DescriptorSet {
        struct UboInfo {
            const Ubo* ubo;
            u32 binding;
            i32 type;
            ShaderStage shaderStage;
        };

        struct SamplerInfo {
            const Texture* texture;
            u32 binding;
            i32 type;
            ShaderStage shaderStage;
        };
    public:
        void create();
        void bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout) const;

        void addSampler(const Texture& texture, u32 binding, ShaderStage stage);
        void addUbo(const Ubo& ubo, u32 binding, ShaderStage stage);

        VkDescriptorSetLayout get() const { return m_descriptorSetLayout; }

    private:
        std::vector<SamplerInfo> m_samplerLayouts;
        std::vector<UboInfo> m_uboLayouts;

        VkDescriptorSetLayout m_descriptorSetLayout = nullptr;

        VkDescriptorSet m_descriptorSets[utl::FRAMES_COUNT] = {};
    };
}
