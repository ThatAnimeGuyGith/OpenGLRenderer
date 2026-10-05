#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
bool initializeGlfw() {
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

GLFWwindow* createOpenGLWindow(int& errorCode) {
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Renderer", nullptr, nullptr);
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
    
    return window;
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    if (!initializeGlfw()) return -1;

    int errorCode = 0;
    GLFWwindow* window = createOpenGLWindow(errorCode);
    if (window == nullptr) return errorCode;

    std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.08f, 0.12f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}