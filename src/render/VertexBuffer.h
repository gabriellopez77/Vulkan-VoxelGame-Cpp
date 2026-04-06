#pragma once

#include "core/Utils.h"


namespace rk {
    enum class UpdateType {
        ONE_TIME,
        OFTEN
    };

    class VertexBuffer {
        struct BufferInfo {
            VkBuffer buffer;
            VkDeviceMemory memory;
            void* mappedMemory;
        };

    public:
        rk::VertexBuffer& createVertices(u32 size,const void* data, UpdateType updateType);
        rk::VertexBuffer& createIndices(u32 size, const u32* data, UpdateType updateType);
        void destroy() const;

        void update(u32 size, const void* data);
        void updateIndices(u32 size, const u32* data);

        void bind(VkCommandBuffer command, u32 binding) const;

    private:
        void update(BufferInfo& bufferInfo, UpdateType updateType, u32 size, const void* data);
        void createSendBuffer(u32 size, const void* data, BufferInfo& bufferInfo, UpdateType updateType, BufferUsage usage);

        u32 m_verticesBufferSize = 0;
        UpdateType m_verticesUpdateType{};
        BufferInfo m_verticesInfo[utl::FRAMES_COUNT] = {};

        u32 m_indicesBufferSize = 0;
        UpdateType m_indicesUpdateType{};
        BufferInfo m_indicesInfo[utl::FRAMES_COUNT] = {};
    };
}
