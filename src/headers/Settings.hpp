#pragma once

#include <string>

struct Settings {
    unsigned int width = 800;
    unsigned int height = 600;
    std::string title = "OpenGL Renderer";
    bool wireframe = false;
    bool vSync = true;
};

// Reads the settings file at `path`.
//
// Keys that are missing keep the defaults above, so a settings file written by
// an older version keeps working after a new setting is added. Keys that are
// present but invalid (wrong type, window size outside 1..16384) throw a
// RendererError in the Config category.
Settings loadSettings(const std::string& path);