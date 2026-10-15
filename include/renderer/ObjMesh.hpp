#pragma once

#include <glad/glad.h>

#include <string>

class ObjMesh {
public:
    explicit ObjMesh(const std::string& path);
    ~ObjMesh();

    ObjMesh(const ObjMesh&) = delete;
    ObjMesh& operator=(const ObjMesh&) = delete;

    void draw() const;

private:
    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    GLsizei vertexCount = 0;
};
