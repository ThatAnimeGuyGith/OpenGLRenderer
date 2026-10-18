#include "RendererUi.hpp"

#include "ModelList.hpp"
#include "Rotation.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <cmath>

namespace {
void normalizeLightDirection(LightingSettings& settings)
{
    const float length = std::sqrt(
        settings.lightDirection[0] * settings.lightDirection[0] +
        settings.lightDirection[1] * settings.lightDirection[1] +
        settings.lightDirection[2] * settings.lightDirection[2]);
    if (length <= 0.0001f) {
        settings.lightDirection[0] = 0.4f;
        settings.lightDirection[1] = 0.8f;
        settings.lightDirection[2] = 0.5f;
        return;
    }
    for (float& component : settings.lightDirection)
        component /= length;
}
}

void resetLightingSettings(LightingSettings& settings)
{
    settings = LightingSettings{};
}

RendererUi::RendererUi(GLFWwindow* window, const std::filesystem::path& iniPath)
    : iniFilename_(iniPath.string())
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = iniFilename_.c_str();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

RendererUi::~RendererUi()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void RendererUi::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

bool RendererUi::draw(
    int& selectedModel,
    RotationState& rotation,
    LightingSettings& lighting,
    std::string_view modelLoadError)
{
    bool modelChanged = false;
    ImGui::Begin("Renderer controls");

    ImGui::TextUnformatted("Model");
    const auto models = builtInModels();
    if (ImGui::BeginCombo("Demo model", models[static_cast<std::size_t>(selectedModel)].name.data())) {
        for (std::size_t index = 0; index < models.size(); ++index) {
            const bool selected = selectedModel == static_cast<int>(index);
            if (ImGui::Selectable(models[index].name.data(), selected)) {
                selectedModel = static_cast<int>(index);
                modelChanged = true;
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (!modelLoadError.empty())
        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%.*s", static_cast<int>(modelLoadError.size()), modelLoadError.data());

    ImGui::Separator();
    ImGui::TextUnformatted("Rotation");
    ImGui::Checkbox("Enable rotation", &rotation.enabled);
    ImGui::SliderFloat("Speed", &rotation.speedRadiansPerSecond, -2.0f, 2.0f, "%.2f rad/s");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Negative values reverse the rotation direction.");

    ImGui::Separator();
    ImGui::TextUnformatted("Lighting");
    ImGui::ColorEdit3("Base color", lighting.baseColor);
    if (ImGui::DragFloat3("Light direction", lighting.lightDirection, 0.01f, -1.0f, 1.0f))
        normalizeLightDirection(lighting);
    ImGui::SliderFloat("Ambient strength", &lighting.ambientStrength, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse strength", &lighting.diffuseStrength, 0.0f, 1.0f);
    if (ImGui::Button("Reset lighting"))
        resetLightingSettings(lighting);

    ImGui::End();
    return modelChanged;
}

void RendererUi::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
