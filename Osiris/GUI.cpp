#include <sstream>
#include <Windows.h>

#include "imgui/imgui.h"
#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Visuals.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

void GUI::render() noexcept
{
    renderMenuBar();
    renderGlowWindow();
    renderVisualsWindow();
    renderMiscWindow();
}

void GUI::checkBoxAndColorPicker(const std::string& name, bool* enable, float* color) noexcept
{
    ImGui::Checkbox(("##" + name).c_str(), enable);
    ImGui::SameLine(0.0f, 5.0f);
    bool openPopup = ImGui::ColorButton(("##" + name + "ColorButton").c_str(), ImColor{ color[0], color[1], color[2] }, ImGuiColorEditFlags_NoTooltip);
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::Text(name.c_str());
    if (openPopup)
        ImGui::OpenPopup(("##" + name + "Popup").c_str());
    if (ImGui::BeginPopup(("##" + name + "Popup").c_str())) {
        ImGui::ColorPicker3(("##" + name + "Picker").c_str(), color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
        ImGui::EndPopup();
    }
}

void GUI::hotkey(int& key) noexcept
{
    ImGui::Text((std::ostringstream{ } << "[ " << (key ? std::to_string(key) : "key") << " ]").str().c_str());
    if (ImGui::IsItemHovered()) {
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
            if (ImGui::IsKeyPressed(i) && i != VK_INSERT)
                key = i != VK_ESCAPE ? i : 0;
    }
}

void GUI::renderMenuBar() noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("glow", nullptr, &window.glow);
        ImGui::MenuItem("visuals", nullptr, &window.visuals);
        ImGui::MenuItem("misc", nullptr, &window.misc);

        if (ImGui::BeginMenu("config")) {
            if (ImGui::MenuItem("load"))
                config.load();
            if (ImGui::MenuItem("save"))
                config.save();
            if (ImGui::MenuItem("reset"))
                config.reset();
            ImGui::EndMenu();
        }
		ImGui::MenuItem("pasted from osiris", nullptr);
        ImGui::EndMainMenuBar();
    }
}

void GUI::renderGlowWindow() noexcept
{
    if (window.glow) {
        ImGui::SetNextWindowSize({ 320.0f, 185.0f });
        ImGui::Begin("glow", &window.glow, windowFlags);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 170.0f);
        ImGui::PushItemWidth(150.0f);
        ImGui::Checkbox("enabled", &config.glow.enabled);
        ImGui::SliderFloat("##1", &config.glow.thickness, 0.0f, 1.0f, "thiccness: %.2f");
        ImGui::SliderFloat("##2", &config.glow.alpha, 0.0f, 1.0f, "alpha: %.2f");
        ImGui::SliderInt("##3", &config.glow.style, 0, 3, "style: %d");
        checkBoxAndColorPicker("allies", &config.glow.allies, config.glow.alliesColor);
        checkBoxAndColorPicker("enemies", &config.glow.enemies, config.glow.enemiesColor);
        ImGui::NextColumn();
        checkBoxAndColorPicker("local player", &config.glow.localPlayer, config.glow.localPlayerColor);
        checkBoxAndColorPicker("weapons", &config.glow.weapons, config.glow.weaponsColor);
        checkBoxAndColorPicker("C4", &config.glow.C4, config.glow.C4Color);
        checkBoxAndColorPicker("planted C4", &config.glow.plantedC4, config.glow.plantedC4Color);
        checkBoxAndColorPicker("chickens", &config.glow.chickens, config.glow.chickensColor);
        ImGui::End();
    }
}

void GUI::renderVisualsWindow() noexcept
{
    if (window.visuals) {
        ImGui::SetNextWindowSize({ 520.0f, 315.0f });
        ImGui::Begin("visuals", &window.visuals, windowFlags);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 210.0f);
        ImGui::Checkbox("no visual recoil", &config.visuals.noVisualRecoil);
        ImGui::Checkbox("no sleeves", &config.visuals.noSleeves);
        ImGui::Checkbox("no smoke", &config.visuals.noSmoke);
        ImGui::Checkbox("no blur", &config.visuals.noBlur);
        ImGui::Checkbox("no scope overlay", &config.visuals.noScopeOverlay);
        ImGui::Checkbox("wireframe smoke", &config.visuals.wireframeSmoke);
        ImGui::NextColumn();
        ImGui::Checkbox("third person", &config.visuals.thirdperson);
        ImGui::SameLine();
        hotkey(config.visuals.thirdpersonKey);
        ImGui::PushItemWidth(290.0f);
        ImGui::SliderInt("##1", &config.visuals.thirdpersonDistance, 0, 1000, "third person distance: %d");
        ImGui::SliderInt("##2", &config.visuals.viewmodelFov, -60, 60, "viewmodel fov: %d");
        ImGui::SliderInt("##3", &config.visuals.flashReduction, 0, 100, "flash reduction: %d%%");
        ImGui::SliderFloat("##4", &config.visuals.brightness, 0.0f, 1.0f, "brightness: %.2f");
        ImGui::PopItemWidth();
        ImGui::Combo("skybox", &config.visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam");
        ImGui::ColorEdit3("world color", config.visuals.worldColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
        ImGui::End();
    }
}

void GUI::renderMiscWindow() noexcept
{
    if (window.misc) {
        ImGui::SetNextWindowSize({ 220.0f, 340.0f });
        ImGui::Begin("misc", &window.misc, windowFlags);
        ImGui::Checkbox("bhop", &config.misc.bunnyHop);
        static char buffer[16];
        ImGui::PushItemWidth(120.0f);
        ImGui::InputText("##1", buffer, IM_ARRAYSIZE(buffer));
        ImGui::SameLine();
        if (ImGui::Button("clantag")) {
            Misc::setClanTag(buffer);
            Misc::animateClanTag(buffer);
        }
        ImGui::Checkbox("animate clantag", &config.misc.animatedClanTag);
        ImGui::Checkbox("spectators", &config.misc.spectatorList);
        ImGui::Checkbox("watermark", &config.misc.watermark);

        if (ImGui::Button("reveal ranks"))
            Misc::revealRanks();
        ImGui::End();
    }
}
