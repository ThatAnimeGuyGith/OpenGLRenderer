#include "Settings.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Settings loadSettings(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open settings file: " + path);
    }

    json data;
    file >> data;

    Settings settings;

    settings.width     = data["window"]["width"];
    settings.height    = data["window"]["height"];
    settings.title     = data["window"]["title"];
    settings.wireframe = data["window"]["wireframe"];

    return settings;
}