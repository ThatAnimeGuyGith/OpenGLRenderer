#include "Shader.hpp"

#include "Error.hpp"
#include "Log.hpp"

#include <fstream>
#include <sstream>
#include <vector>

std::string loadShaderSource(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw RendererError(
            ErrorCategory::Io,
            "Failed to open shader file: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source, const std::string& name)
{
    const GLuint shader = glCreateShader(type);
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::string infoLog;
        if (logLength > 0) {
            std::vector<char> buffer(static_cast<size_t>(logLength));
            glGetShaderInfoLog(shader, logLength, nullptr, buffer.data());
            infoLog.assign(buffer.data());
        }

        glDeleteShader(shader);
        throw RendererError(
            ErrorCategory::Shader,
            "Failed to compile " + name + ":\n" + infoLog);
    }

    Log::info("Compiled shader {}", name);
    return shader;
}

GLuint createShaderProgram(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath)
{
    const std::string vertexSource = loadShaderSource(vertexPath);
    const std::string fragmentSource = loadShaderSource(fragmentPath);

    const GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath.string());

    GLuint fragmentShader = 0;
    try {
        fragmentShader =
            compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath.string());
    } catch (...) {
        glDeleteShader(vertexShader);
        throw;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        std::string infoLog;
        if (logLength > 0) {
            std::vector<char> buffer(static_cast<size_t>(logLength));
            glGetProgramInfoLog(program, logLength, nullptr, buffer.data());
            infoLog.assign(buffer.data());
        }

        glDeleteProgram(program);
        throw RendererError(
            ErrorCategory::Shader,
            "Failed to link shader program:\n" + infoLog);
    }

    Log::info("Linked shader program");
    return program;
}

void setUniformMatrix4(GLuint program, const char* name, const float* value)
{
    const GLint location = glGetUniformLocation(program, name);
    if (location >= 0)
        glUniformMatrix4fv(location, 1, GL_FALSE, value);
}
