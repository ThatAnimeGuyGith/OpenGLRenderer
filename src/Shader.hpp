#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <string>

std::string loadShaderSource(const std::filesystem::path& path);

GLuint compileShader(GLenum type, const std::string& source, const std::string& name);

GLuint createShaderProgram(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath);
