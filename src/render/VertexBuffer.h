#pragma once

#include "core/Utils.h"


namespace rk {
    enum class UpdateType {
        ONE_TIME,
        SOMETIMES,
        OFTEN,
    };

    class VertexBuffer {
        struct BufferInfo {
            // if ONE_TIMES or OFTEN then it is nullptr
            // if SOMETIMES then it is staging buffer used to copy data to vram buffer
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingMemory;

            // if ONE_TIME or SOMETIMES then it is vram buffer
            // if OFTEN then it is host local buffer
            VkBuffer buffer;
            VkDeviceMemory memory;

            // if ONE_TIMES then it is nullptr
            // if SOMETIMES then it is staging buffer mapped memory
            // if OFTEN then it is buffer mapped memory
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
        void createOrSendBuffer(u32 size, const void* data, BufferInfo& bufferInfo, UpdateType updateType, BufferUsage usage);

        u32 m_verticesBufferSize = 0;
        UpdateType m_verticesUpdateType{};
        BufferInfo m_verticesInfo[utl::FRAMES_COUNT] = {};

        u32 m_indicesBufferSize = 0;
        UpdateType m_indicesUpdateType{};
        BufferInfo m_indicesInfo[utl::FRAMES_COUNT] = {};
    };
}
