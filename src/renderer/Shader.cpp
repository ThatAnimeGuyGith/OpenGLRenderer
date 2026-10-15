#include "renderer/Shader.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <utility>
#include <vector>

namespace {
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
    if (shader == 0)
        throw std::runtime_error(std::string("Failed to create shader: ") + name);

    const char* sourcePtr = source.c_str();

    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(static_cast<std::size_t>(logLength > 0 ? logLength : 1));
        glGetShaderInfoLog(shader, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());
        glDeleteShader(shader);
        throw std::runtime_error(
            std::string("Shader compilation failed (") + name + "):\n" + infoLog.data()
        );
    }

    return shader;
}
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    const std::string vertexSource = loadShaderSource(vertexPath);
    const std::string fragmentSource = loadShaderSource(fragmentPath);

    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint program = 0;

    try {
        vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
        fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

        program = glCreateProgram();
        if (program == 0)
            throw std::runtime_error("Failed to create shader program");

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> infoLog(static_cast<std::size_t>(logLength > 0 ? logLength : 1));
            glGetProgramInfoLog(program, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());
            throw std::runtime_error(std::string("Shader linking failed:\n") + infoLog.data());
        }
    }
    catch (...) {
        if (program != 0)
            glDeleteProgram(program);
        if (vertexShader != 0)
            glDeleteShader(vertexShader);
        if (fragmentShader != 0)
            glDeleteShader(fragmentShader);
        throw;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    programId = program;
}

Shader::~Shader()
{
    if (programId != 0)
        glDeleteProgram(programId);
}

Shader::Shader(Shader&& other) noexcept
    : programId(std::exchange(other.programId, 0)),
      uniformLocations(std::move(other.uniformLocations))
{}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this == &other)
        return *this;

    if (programId != 0)
        glDeleteProgram(programId);

    programId = std::exchange(other.programId, 0);
    uniformLocations = std::move(other.uniformLocations);
    return *this;
}

void Shader::use() const
{
    glUseProgram(programId);
}

GLuint Shader::id() const noexcept
{
    return programId;
}

GLint Shader::getUniformLocation(const std::string& name) const
{
    const auto found = uniformLocations.find(name);
    if (found != uniformLocations.end())
        return found->second;

    const GLint location = glGetUniformLocation(programId, name.c_str());
    uniformLocations.emplace(name, location);
    return location;
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setMat4(const std::string& name, const float* value) const
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value);
}