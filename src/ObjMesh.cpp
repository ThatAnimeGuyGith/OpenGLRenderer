#include "ObjMesh.hpp"

#include "Error.hpp"
#include "Log.hpp"
#include "ObjMeshData.hpp"
#include "ObjPaths.hpp"

#include <tiny_obj_loader.h>

#include <cstddef>
#include <limits>
#include <vector>

ObjMesh::ObjMesh(const std::filesystem::path& path)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;
    const std::string pathString = path.string();
    const std::string basePath = objMaterialBasePath(path).generic_string() + "/";
    if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, pathString.c_str(), basePath.c_str(), true))
        throw RendererError(ErrorCategory::Io, "Failed to load OBJ mesh '" + pathString + "': " + error);
    if (!warning.empty())
        Log::warn("OBJ loader warning for '{}': {}", pathString, warning);
    std::vector<ObjMeshIndex> indices;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices)
            indices.push_back({index.vertex_index, index.normal_index});
    }
    const std::vector<ProcessedMeshVertex> vertices = processObjMeshVertices(
        attributes.vertices, attributes.normals, indices, pathString);
    if (vertices.empty() || vertices.size() > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()))
        throw RendererError(ErrorCategory::Io, "OBJ mesh has no drawable triangles: " + pathString);

    glGenVertexArrays(1, &vertexArray_);
    glGenBuffers(1, &vertexBuffer_);
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(ProcessedMeshVertex)), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ProcessedMeshVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ProcessedMeshVertex), reinterpret_cast<void*>(offsetof(ProcessedMeshVertex, normal)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    vertexCount_ = static_cast<GLsizei>(vertices.size());
    Log::info("Loaded OBJ mesh '{}' with {} vertices", pathString, vertexCount_);
}

ObjMesh::~ObjMesh()
{
    if (vertexBuffer_ != 0)
        glDeleteBuffers(1, &vertexBuffer_);
    if (vertexArray_ != 0)
        glDeleteVertexArrays(1, &vertexArray_);
}

void ObjMesh::draw() const
{
    glBindVertexArray(vertexArray_);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
}
