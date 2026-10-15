#include "platform/Window.hpp"

#include <glad/glad.h>

#include <cstdio>
#include <stdexcept>

namespace {
void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}
}

Window::Window(const std::string& title, unsigned int width, unsigned int height)
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (handle == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create an OpenGL window");
    }

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(handle);
        handle = nullptr;
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetFramebufferSizeCallback(handle, framebufferSizeCallback);

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(handle, &framebufferWidth, &framebufferHeight);
    framebufferSizeCallback(handle, framebufferWidth, framebufferHeight);

    std::printf(
        "Successfully created an OpenGL window: \"%s\" (%u x %u)\n",
        title.c_str(),
        width,
        height
    );
}

Window::~Window()
{
    if (handle != nullptr)
        glfwDestroyWindow(handle);
    glfwTerminate();
}

GLFWwindow* Window::get() const noexcept
{
    return handle;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
