#pragma once

#include <array>
#include <string_view>
#include <vector>

struct ObjMeshIndex {
    int vertexIndex;
    int normalIndex;
};

struct ProcessedMeshVertex {
    std::array<float, 3> position;
    std::array<float, 3> normal;
};

std::vector<ProcessedMeshVertex> processObjMeshVertices(
    const std::vector<float>& positions,
    const std::vector<float>& normals,
    const std::vector<ObjMeshIndex>& indices,
    std::string_view meshName);
