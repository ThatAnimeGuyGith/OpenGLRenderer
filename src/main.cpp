#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Error.hpp"
#include "Log.hpp"
#include "Paths.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "Window.hpp"

int main()
{
    try {
        const Settings settings =
            loadSettings(assetPath("config/settings.json").string());

        const GlfwSession glfw;
        GLFWwindow* window = createOpenGLWindow(
            settings.title,
            settings.width,
            settings.height);

        if (settings.wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        Log::info(
            "OpenGL version: {}",
            reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        Log::info("OpenGL Renderer {}", OPENGLRENDERER_VERSION);

        const GLuint shaderProgram = createShaderProgram(
            assetPath("shaders/vertexShader.vert"),
            assetPath("shaders/fragmentShader.frag"));

        const float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        GLuint VAO = 0;
        GLuint VBO = 0;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        while (!glfwWindowShouldClose(window)) {
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
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        return 0;
    } catch (const RendererError& e) {
        Log::error(e.what());
        return exitCode(e.category());
    } catch (const std::exception& e) {
        Log::error(e.what());
        return 1;
    }
}
