#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>

class GlfwSession {
public:
    GlfwSession();
    ~GlfwSession();

    GlfwSession(const GlfwSession&) = delete;
    GlfwSession& operator=(const GlfwSession&) = delete;
};

GLFWwindow* createOpenGLWindow(
    const std::string& title,
    unsigned int width,
    unsigned int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
