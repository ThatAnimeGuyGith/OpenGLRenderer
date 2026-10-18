#pragma once

#include <cstddef>
#include <span>
#include <string_view>

struct BuiltInModel {
    std::string_view name;
    std::string_view assetPath;
};

std::span<const BuiltInModel> builtInModels();
bool isBuiltInModelIndex(std::size_t index);
