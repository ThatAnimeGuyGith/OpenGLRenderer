#include "Window.hpp"

#include <glad/glad.h>

#include <cstdio>
#include <string>

bool initializeGlfw()
{
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

GLFWwindow* createOpenGLWindow(int& errorCode, std::string name, unsigned int WIDTH, unsigned int HEIGHT)
{
    GLFWwindow* window =
        glfwCreateWindow(WIDTH, HEIGHT, name.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        std::fprintf(stderr, "Failed to create an OpenGL window\n");
        glfwTerminate();

        errorCode = -2;
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to initialize GLAD\n");

        glfwDestroyWindow(window);
        glfwTerminate();

        errorCode = -3;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::printf(
        "Successfully created an OpenGL window: \"%s\" (%dx%d)\n",
        name.c_str(),
        WIDTH,
        HEIGHT
    );

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