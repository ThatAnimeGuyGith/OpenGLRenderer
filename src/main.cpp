#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdio>
#include <exception>
#include <iostream>
#include <stdexcept>

#include <config/Settings.hpp>
#include <platform/Window.hpp>
#include <renderer/ObjMesh.hpp>
#include <renderer/Shader.hpp>

namespace {
struct RenderState {
    Shader* shader;
    ObjMesh* mesh;
};

void drawFrame(GLFWwindow* window, const RenderState& state)
{
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    if (framebufferWidth <= 0 || framebufferHeight <= 0)
        return;

    constexpr float pi = 3.14159265358979323846f;
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = 20.0f;
    const float aspect = static_cast<float>(framebufferWidth) /
                         static_cast<float>(framebufferHeight);
    const float focalLength = 1.0f / std::tan(45.0f * pi / 360.0f);
    const float projection[] = {
        focalLength / aspect, 0.0f,         0.0f,                                  0.0f,
        0.0f,                 focalLength,  0.0f,                                  0.0f,
        0.0f,                 0.0f,         (farPlane + nearPlane) / (nearPlane - farPlane), -1.0f,
        0.0f,                 0.0f,         (2.0f * farPlane * nearPlane) / (nearPlane - farPlane), 0.0f
    };
    const float time = static_cast<float>(glfwGetTime()) * 0.35f;
    const float cosineY = std::cos(time);
    const float sineY = std::sin(time);
    const float cosineX = std::cos(-0.12f);
    const float sineX = std::sin(-0.12f);
    const float model[] = {
        cosineY, sineY * sineX, -sineY * cosineX, 0.0f,
        0.0f,    cosineX,       sineX,            0.0f,
        sineY,   -cosineY * sineX, cosineY * cosineX, 0.0f,
        0.0f,    0.0f,            0.0f,            1.0f
    };
    const float view[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -3.2f, 1.0f
    };

    glClearColor(0.08f, 0.12f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    state.shader->use();
    state.shader->setMat4("model", model);
    state.shader->setMat4("view", view);
    state.shader->setMat4("projection", projection);
    state.mesh->draw();
    glfwSwapBuffers(window);
}

void refreshWindow(GLFWwindow* window)
{
    auto* state = static_cast<RenderState*>(glfwGetWindowUserPointer(window));
    if (state != nullptr)
        drawFrame(window, *state);
}

void render(GLFWwindow* window)
{
    Shader shader(
        "./assets/shaders/vertexShader.vert",
        "./assets/shaders/fragmentShader.frag"
    );
    ObjMesh bunny("./assets/meshes/stanfordBunny/stanfordBunny.obj");
    RenderState renderState{&shader, &bunny};
    glfwSetWindowUserPointer(window, &renderState);
    glfwSetWindowRefreshCallback(window, refreshWindow);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        drawFrame(window, renderState);
        glfwPollEvents();
    }

    glfwSetWindowRefreshCallback(window, nullptr);
    glfwSetWindowUserPointer(window, nullptr);
}
}

int main()
{
    try {
        const Settings settings = loadSettings("config/settings.json");
        Window window(settings.title, settings.width, settings.height);

        glEnable(GL_DEPTH_TEST);
        glPolygonMode(
            GL_FRONT_AND_BACK,
            settings.wireframe ? GL_LINE : GL_FILL
        );
        std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));
        render(window.get());
    }
    catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return -4;
    }

    return 0;
}