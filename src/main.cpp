#include "Application.h"


i32 main() {
    Application app;
    
    app.initWindow(1000, 800, "Vulkan App");
    app.initVulkan();
    app.run();
    app.clearVulkan();
}