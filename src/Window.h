#pragma once

#include "Defs.h"


// fwd
struct GLFWwindow;

namespace window {
    enum class CursorMode {
        Normal = 0x00034001,
        Disabled = 0x00034003
    };

    extern f32 DeltaTime;
    extern f32 Time;

    extern void initWindow(i32 width, i32 height, const char* title);
    extern void run();
    extern void clear();
    extern void setCursorMode(CursorMode mode);

    extern GLFWwindow* getGlfwWindow();
    extern i32 getWindowWidth();
    extern i32 getWindowHeight();

    extern void getFrameBufferSize(i32* width, i32* height);
}