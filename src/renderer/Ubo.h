#pragma once

#include <array>

#include "Defs.h"
#include "VulkanFwd.h"
#include "Utils.h"


namespace rk {
    // fwd
    class VulkanApp;

    class Ubo {
    public:
        void create(u64 size);
        void update(u64 offset, u64 size, const void* data) const;
        void destroy();

        VkBuffer getBuffer(int i) const { return m_buffers[i]; }
        u32 getSize() const { return m_size; }

    private:
        u32 m_size = 0;
        std::array<VkBuffer, utl::FRAMES_COUNT> m_buffers{};
        std::array<VkDeviceMemory, utl::FRAMES_COUNT> m_buffersMemory{};
        std::array<void*, utl::FRAMES_COUNT> m_buffersMapped{};
    };
}
