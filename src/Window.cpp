#include "Window.hpp"

#include "Error.hpp"
#include "Log.hpp"

#include <glad/glad.h>

namespace {
// GLFW describes why a call failed through this callback, for example "the
// driver does not support OpenGL 3.3". Without it the only symptom is a null
// window. Failures are also reported through exceptions, so this only logs.
void glfwErrorCallback(int code, const char* description)
{
    Log::warn("GLFW error {:#x}: {}", code, description);
}
}

GlfwSession::GlfwSession()
{
    // Allowed before glfwInit, and needed to see why glfwInit itself fails.
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        throw RendererError(ErrorCategory::Window, "Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    // macOS only provides a 3.2+ core context when forward compatibility is on.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
}

GlfwSession::~GlfwSession()
{
    glfwTerminate();
}

GLFWwindow* createOpenGLWindow(
    const std::string& title,
    unsigned int width,
    unsigned int height,
    bool vSync)
{
    GLFWwindow* window =
        glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        throw RendererError(ErrorCategory::Window, "Failed to create an OpenGL window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(vSync ? 1 : 0);

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
