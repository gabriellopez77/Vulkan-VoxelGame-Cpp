#pragma once

#include "Defs.h"
#include "Utils.h"

namespace rk {
    // fwd
    class VulkanApp;
    class Ubo;

    class DescriptorSetLayout {
    public:
        void create(const VulkanApp* app, u32 binding, u32 stage);
        void bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout, u32 currentFrame);
        void setUbo(const VulkanApp* app, const Ubo& ubo, u32 binding);

        VkDescriptorSetLayout get() const { return m_descriptorSetLayout; }

    private:
        VkDescriptorSetLayout m_descriptorSetLayout = nullptr;

        VkDescriptorSet m_descriptorSets[utl::FRAMES_COUNT] = {};
    };
}
