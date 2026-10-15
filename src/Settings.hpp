#pragma once

#include <string>

struct Settings {
    unsigned int width = 800;
    unsigned int height = 600;
    std::string title;
    bool wireframe = false;
};

Settings loadSettings(const std::string& path);