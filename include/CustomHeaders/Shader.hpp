#pragma once

#include <glad/glad.h>
#include <string>

std::string loadShaderSource(const char* filepath);

GLuint compileShader(GLenum type, const std::string& source, const char* name);

GLuint createShaderProgram(
    const char* vertexPath,
    const char* fragmentPath
);