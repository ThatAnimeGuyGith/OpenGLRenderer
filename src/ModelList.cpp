#include "ModelList.hpp"

#include <array>

std::span<const BuiltInModel> builtInModels()
{
    static constexpr std::array models{
        BuiltInModel{"Cube", "meshes/cube/cube.obj"},
        BuiltInModel{"Pyramid", "meshes/pyramid/pyramid.obj"}
    };
    return models;
}

bool isBuiltInModelIndex(std::size_t index)
{
    return index < builtInModels().size();
}
