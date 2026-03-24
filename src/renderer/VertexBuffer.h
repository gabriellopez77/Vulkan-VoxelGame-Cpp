#pragma once

#include "VulkanFwd.h"

#include "../defs.h"

//fwd

namespace render {
    class VulkanApp;

    class VertexBuffer {
    public:
        void create(const VulkanApp* app, u64 size, const void* data);
        void destroy(VkDevice device) const;
        void bind(VkCommandBuffer command) const;

    private:
        u32 findMemoryType(const VulkanApp* app, u32 type, u32 properties);

        VkBuffer m_buffer = nullptr;
        VkDeviceMemory m_memory = nullptr;
    };
}
