#include "Shader.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

std::string loadShaderSource(const char* filepath)
{
    std::ifstream file(filepath);

    if (!file.is_open()) {
        throw std::runtime_error(
            std::string("Failed to open shader file: ") + filepath
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source, const char* name)
{
    GLuint shader = glCreateShader(type);

    const char* sourcePtr = source.c_str();

    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512];

        glGetShaderInfoLog(
            shader,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Shader compilation failed (" << name << "):\n"
                  << infoLog << '\n';

        glDeleteShader(shader);
        return 0;
    }
    else {
        std::printf("Shader compiled successfully: %s\n", name);
    }

    return shader;
}

GLuint createShaderProgram(
    const char* vertexPath,
    const char* fragmentPath
)
{
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);

    GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);

    GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        std::cerr << "Shader linking failed:\n"
                  << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}