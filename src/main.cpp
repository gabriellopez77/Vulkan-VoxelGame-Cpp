#include "Window.h"


i32 main() {
    window::initWindow(1000, 800, "Vulkan App");
    window::run();
    window::clear();
}