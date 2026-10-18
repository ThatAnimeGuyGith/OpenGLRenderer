#pragma once

#include <filesystem>
#include <string>
#include <string_view>

struct GLFWwindow;
struct RotationState;

struct LightingSettings {
    float baseColor[3]{0.82f, 0.62f, 0.38f};
    float lightDirection[3]{0.4f, 0.8f, 0.5f};
    float ambientStrength = 0.24f;
    float diffuseStrength = 0.76f;
};

void resetLightingSettings(LightingSettings& settings);

class RendererUi {
public:
    RendererUi(GLFWwindow* window, const std::filesystem::path& iniPath);
    ~RendererUi();

    RendererUi(const RendererUi&) = delete;
    RendererUi& operator=(const RendererUi&) = delete;

    void beginFrame();
    bool draw(
        int& selectedModel,
        RotationState& rotation,
        LightingSettings& lighting,
        std::string_view modelLoadError);
    void render();

private:
    std::string iniFilename_;
};
