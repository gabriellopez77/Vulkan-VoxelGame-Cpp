#pragma once

#include "core/VulkanFwd.h"
#include "VulkanEnums.h"


namespace rk {
    enum class VertexBufferType {
        ONE_TIME,
        RAM,
    };

    class VertexBuffer {
        struct BufferInfo {
            u32 size;
            VertexBufferType updateType;
            VkBuffer buffer;
            VkDeviceMemory memory;
            void* mappedMemory;
        };

    public:
        rk::VertexBuffer& createVertices(u32 size,const void* data, VertexBufferType updateType);
        rk::VertexBuffer& createIndices(u32 size, const u32* data, VertexBufferType updateType);
        void destroy() const;

        void update(u32 size, const void* data) { update(m_verticesInfo, size, data); }
        void updateIndices(u32 size, const u32* data) { update(m_indicesInfo, size, data); }

        void bind(VkCommandBuffer command, u32 binding) const;

    private:
        void update(BufferInfo& bufferInfo, u32 size, const void* data);
        void createSendBuffer(const void* data, BufferInfo& bufferInfo, BufferUsage usage);

        BufferInfo m_verticesInfo;
        BufferInfo m_indicesInfo;
    };
}
