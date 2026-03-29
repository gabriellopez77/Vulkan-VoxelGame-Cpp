#pragma once

#include "VulkanFwd.h"

#include "Defs.h"


namespace rk {
    //fwd
    class VulkanApp;

    class VertexBuffer {
    public:
        void create(const VulkanApp* app, u64 verticesSize,
            const void* verticesData, u64 indicesSize,
            const void* indicesData, i32 indexType);
        void destroy(VkDevice device) const;
        void bind(VkCommandBuffer command) const;

    private:
        VkBuffer m_buffer = nullptr;
        VkDeviceMemory m_memory = nullptr;

        VkBuffer m_indexBuffer = nullptr;
        VkDeviceMemory m_indexMemory = nullptr;
        i32 m_indexBufferType = 0;
    };
}
