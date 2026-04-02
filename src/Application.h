#pragma once

#include "renderer/Defs.h"


// fwd
struct GLFWwindow;
class Game;

class Application {
public:
    enum class CursorMode {
        Normal = 0x00034001,
        Disabled = 0x00034003
    };

    static f32 DeltaTime;
    static f32 Time;

    void initWindow(i32 width, i32 height, const char* title);
    void run(Game* game);
    void clear();
    void setCursorMode(CursorMode mode);

    i32 getWindowWidth() const { return m_windowWidth; }
    i32 getWindowHeight() const { return m_windowHeight; }

private:
    static void resizeCallback(GLFWwindow* window, i32 width, i32 height);
    static void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double x, double y);

    Game* m_game = nullptr;

    // glfw window pointer
    GLFWwindow* m_window = nullptr;
    i32 m_windowWidth = 0;
    i32 m_windowHeight = 0;

    // last frame time
    f32 m_lastFrame = 0.f;
};