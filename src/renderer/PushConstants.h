#pragma once

#include <vulkan/vulkan.h>

#include "Defs.h"


namespace rk {
    // fwd
    enum class ShaderStage : i32;

    class PushConstants {
    public:
        void create(u32 size, ShaderStage stage);

        void bind(VkCommandBuffer command, VkPipelineLayout layout, u32 size, const void *data) const {
            vkCmdPushConstants(command, layout, m_pushConstantInfo.stageFlags, 0, size, data);
        }

        const VkPushConstantRange& get() const { return m_pushConstantInfo; }

    private:
        VkPushConstantRange m_pushConstantInfo{};
    };
}