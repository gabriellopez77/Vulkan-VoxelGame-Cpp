#pragma once

#include "VulkanFwd.h"
#include "VulkanEnums.h"


namespace rk {
    class RawTexture {
    public:
        void create(str texturePath, SamplerFilter filter, SamplerMode mode);
        void create(const void* data, i32  width, i32 height, SamplerFilter filter, SamplerMode mode);
        void destroy() const;

        VkSampler getSampler() const { return m_sampler; }
        VkImageView getImageView() const { return m_imageView; }

    private:
        void createSampler(SamplerFilter filter, SamplerMode mode);

        VkImage m_image = nullptr;
        VkDeviceMemory m_imageMemory = nullptr;
        VkImageView m_imageView = nullptr;
        VkSampler m_sampler = nullptr;
    };
}
