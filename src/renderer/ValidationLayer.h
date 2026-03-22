#pragma once


// fwd
class VulkanApp;

class ValidationLayer {
public:
    void init(const VulkanApp* app);

private:
    bool checkSupport();
};
