#pragma once

#include "VulkanFwd.h"


namespace rk {
    // fwd
    class VulkanApp;

    class Texture {
    public:
        void create(const char* texturePath);

    private:
        VkImage textureImage = nullptr;
        VkDeviceMemory textureImageMemory = nullptr;
    };
}
