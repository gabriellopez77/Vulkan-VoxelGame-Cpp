#pragma once

#include "renderer/Defs.h"


// fwd
struct GLFWwindow;

class Application {
public:
    static f32 DeltaTime;
    static f32 Time;

    void initWindow(i32 width, i32 height, const char* title);
    void initVulkan();
    void clearVulkan();
    void run();

    i32 getWindowWidth() const { return m_windowWidth; }
    i32 getWindowHeight() const { return m_windowHeight; }

    void setLoopFunc(void (*loopFunc)(float dt)) { m_loopFunc = loopFunc; }
    void setStartFunc(void (*startFunc)()) { m_startFunc = startFunc; }
    void setRenderFunc(void (*renderFunc)()) { m_renderFunc = renderFunc; }
    void setResizeFunc(void (*resizeFun)(i32 width, i32 height)) {m_resizeFunc = resizeFun; }

private:
    static void resizeCallback(GLFWwindow* window, i32 width, i32 height);

    // functions pointers
    void (*m_startFunc)() = nullptr;
    void (*m_loopFunc)(float dt) = nullptr;
    void (*m_renderFunc)() = nullptr;
    void (*m_resizeFunc)(i32 width, i32 height) = nullptr;

    // glfw window pointer
    GLFWwindow* m_window = nullptr;
    i32 m_windowWidth = 0;
    i32 m_windowHeight = 0;

    // last frame time
    f32 m_lastFrame = 0.f;
};