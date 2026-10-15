#pragma once

#include <filesystem>

std::filesystem::path executableDir();
std::filesystem::path assetPath(const std::filesystem::path& relative);
