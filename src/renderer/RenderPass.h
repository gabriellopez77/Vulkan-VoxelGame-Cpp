#pragma once


//fwd
class VulkanApp;
typedef struct VkRenderPass_T *VkRenderPass;

class RenderPass {
public:
    void init(const VulkanApp* app);

    VkRenderPass get() const { return m_renderPass; }

private:
    VkRenderPass m_renderPass = nullptr;
};
