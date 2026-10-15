#include "Settings.hpp"

#include "Error.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Settings loadSettings(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        throw RendererError(
            ErrorCategory::Config,
            "Could not open settings file: " + path);
    }

    json data;
    try {
        file >> data;
        const json& window = data.at("window");

        Settings settings;
        settings.width = window.at("width").get<unsigned int>();
        settings.height = window.at("height").get<unsigned int>();
        settings.title = window.at("title").get<std::string>();
        settings.wireframe = window.at("wireframe").get<bool>();
        return settings;
    } catch (const json::exception& e) {
        throw RendererError(
            ErrorCategory::Config,
            path + ": " + e.what());
    }
}
