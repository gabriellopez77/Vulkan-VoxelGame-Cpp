#include "Application.h"

#include <GLFW/glfw3.h>

#include <cassert>

#include "renderer/VulkanApp.h"


f32 Application::DeltaTime = 0.f;
f32 Application::Time = 0.f;

void Application::initWindow(i32 width, i32 height, const char* title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    m_windowWidth = width;
    m_windowHeight = height;

    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, resizeCallback);
}

void Application::initVulkan() {
    rk::VulkanApp::init(m_window);
}

void Application::clearVulkan() {
    rk::VulkanApp::get()->clear();
}

void Application::run() {
    if (!m_window) {
        assert(false && "Window not initialized. Call initWindow() before run().");
    }

    if (m_startFunc)
        m_startFunc();

    while (!glfwWindowShouldClose(m_window)) {
        // poll events
        glfwPollEvents();

        // calculate delta time
        float time = (f32)glfwGetTime();
        DeltaTime = time - m_lastFrame;
        m_lastFrame = time;
        Time = time;

        // run the main loop function if it's set
        if (m_loopFunc) {
            m_loopFunc(DeltaTime);
        }

        rk::VulkanApp::get()->beginFrame();

        // run the render function if it's set
        if (m_renderFunc)
            m_renderFunc();

        rk::VulkanApp::get()->endFrame();
    }
}

void Application::resizeCallback(GLFWwindow* window, i32 width, i32 height) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    if (app->m_resizeFunc)
        app->m_resizeFunc(width, height);

    app->m_windowWidth = width;
    app->m_windowHeight = height;

    rk::VulkanApp::get()->resize();
}