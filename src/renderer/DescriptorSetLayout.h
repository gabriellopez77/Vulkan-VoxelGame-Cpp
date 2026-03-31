#pragma once

#include "Defs.h"
#include "Utils.h"


namespace rk {
    // fwd
    class VulkanApp;
    class Ubo;
    enum class ShaderStage : i32;

    class DescriptorSetLayout {
    public:
        void create(u32 binding, ShaderStage stage);
        void bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout);
        void setUbo(const Ubo& ubo, u32 binding);

        VkDescriptorSetLayout get() const { return m_descriptorSetLayout; }

    private:
        VkDescriptorSetLayout m_descriptorSetLayout = nullptr;

        VkDescriptorSet m_descriptorSets[utl::FRAMES_COUNT] = {};
    };
}
