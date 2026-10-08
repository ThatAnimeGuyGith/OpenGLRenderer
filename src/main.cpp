#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>

#include <CustomHeaders/settings.hpp>
#include <CustomHeaders/Window.hpp>
#include <CustomHeaders/Shader.hpp>

int main()
{
    Settings settings = loadSettings("config/settings.json");

    if (!initializeGlfw())
        return -1;

    int errorCode = 0;

    GLFWwindow* window = createOpenGLWindow(errorCode, settings.title, settings.width, settings.height);
    if (window == nullptr)
        return errorCode;

    if (settings.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    std::printf(
        "OpenGL version: %s\n",
        glGetString(GL_VERSION)
    );

    GLuint shaderProgram = createShaderProgram(
        "./shaders/vertexShader.vert",
        "./shaders/fragmentShader.frag"
    );

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    GLuint VAO;
    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // We don't need to keep these bound after setup.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.08f, 0.12f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (shaderProgram == 0)
        return -4;
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}