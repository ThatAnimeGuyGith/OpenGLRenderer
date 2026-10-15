#pragma once

#include <filesystem>
#include <glad/glad.h>

class ObjMesh {
public:
    explicit ObjMesh(const std::filesystem::path& path);
    ~ObjMesh();

    ObjMesh(const ObjMesh&) = delete;
    ObjMesh& operator=(const ObjMesh&) = delete;

    void draw() const;

private:
    GLuint vertexArray_ = 0;
    GLuint vertexBuffer_ = 0;
    GLsizei vertexCount_ = 0;
};