#include "Window.hpp"

#include "Error.hpp"
#include "Log.hpp"

#include <glad/glad.h>

GlfwSession::GlfwSession()
{
    if (!glfwInit()) {
        throw RendererError(ErrorCategory::Window, "Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GlfwSession::~GlfwSession()
{
    glfwTerminate();
}

GLFWwindow* createOpenGLWindow(
    const std::string& title,
    unsigned int width,
    unsigned int height)
{
    GLFWwindow* window =
        glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        throw RendererError(ErrorCategory::Window, "Failed to create an OpenGL window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(window);
        throw RendererError(ErrorCategory::Graphics, "Failed to initialize GLAD");
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int fbWidth = 0;
    int fbHeight = 0;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    Log::info(
        "Created OpenGL window \"{}\" ({}x{}, framebuffer {}x{})",
        title,
        width,
        height,
        fbWidth,
        fbHeight);

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
