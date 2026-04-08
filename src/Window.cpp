#include "Window.h"

#include <GLFW/glfw3.h>

#include <cassert>

#include "render/core/VulkanApp.h"

#include "Game.h"
#include "inputs.h"


// window callbacks
static void resizeCallback(GLFWwindow* window, i32 width, i32 height);
static void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
static void mouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods);
static void cursorPosCallback(GLFWwindow* window, f64 x, f64 y);


namespace window {
    f32 DeltaTime = 0.f;
    f32 Time = 0.f;

    // glfw window pointer
    GLFWwindow* glfwWindow = nullptr;

    i32 windowWidth = 0;
    i32 windowHeight = 0;

    // last frame time
    f32 lastFrame = 0.f;


    extern GLFWwindow* getGlfwWindow() { return glfwWindow; };
    extern i32 getWindowWidth() { return windowWidth; }
    extern i32 getWindowHeight() { return windowHeight; }


    void initWindow(i32 width, i32 height, const char* title) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        windowWidth = width;
        windowHeight = height;

        glfwSetFramebufferSizeCallback(glfwWindow, resizeCallback);
        glfwSetKeyCallback(glfwWindow, keyCallback);
        glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
        glfwSetCursorPosCallback(glfwWindow, cursorPosCallback);

        setCursorMode(CursorMode::Disabled);
    }

    void clear() {
        rk::vulkanApp::clear();
    }

    void run() {
        if (!glfwWindow) {
            assert(false && "Window not initialized. Call initWindow() before run().");
        }

        // init vulkan
        rk::vulkanApp::init();

        // init game
        game::start();
        game::resize(windowWidth, windowHeight);

        while (!glfwWindowShouldClose(glfwWindow)) {
            inputs::newFrame();

            // poll events
            glfwPollEvents();


            // calculate delta time
            f32 time = (f32)glfwGetTime();
            DeltaTime = time - lastFrame;
            lastFrame = time;
            Time = time;

            // run game loop
            game::update(DeltaTime);

            rk::vulkanApp::beginFrame();

            // run game render
            game::render();

            rk::vulkanApp::endFrame();
        }
    }

    void setCursorMode(CursorMode mode) {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, (int)mode);
    }

    void getFrameBufferSize(i32* width, i32* height) {
        glfwGetFramebufferSize(glfwWindow, width, height);
    }
}


void resizeCallback(GLFWwindow* window, i32 width, i32 height) {
    rk::vulkanApp::resize();

    game::resize(width, height);

    window::windowWidth = width;
    window::windowHeight = height;

}

void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
    if (key == GLFW_KEY_UNKNOWN)
        return;

    inputs::setKeyState(key, action != GLFW_RELEASE);
}

void mouseButtonCallback(GLFWwindow *window, i32 button, i32 action, i32 mods) {
    inputs::setMouseButtonState(button, action != GLFW_RELEASE);
}

void cursorPosCallback(GLFWwindow* window, f64 x, f64 y) {
    inputs::setMousePos({ (f32)x, (f32)y });
}
