#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>



bool initializeGlfw();
GLFWwindow* createOpenGLWindow(int& errorCode, std::string name, unsigned int WIDTH, unsigned int HEIGHT);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);