#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(const std::string& title, unsigned int width, unsigned int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    GLFWwindow* get() const noexcept;

private:
    GLFWwindow* handle = nullptr;
};

void processInput(GLFWwindow* window);
