#pragma once
#include "InfoItem.h"
#include <string>

struct counter_element {
    ImVec4 color;
};

class CounterItem : public InfoItem
{
public:
    CounterItem() {
        windowTitle = "Counter";
        refreshIntervalMs = 50;
    }
    ~CounterItem() {}

    void Update() override;
    void DrawContent() override;

    void Load(const nlohmann::json& j) override;
    void Save(nlohmann::json& j) const override;

    int count = 0;
    int lastCount = 0;
    float fontSize = 20.0f;

	bool isPlaySound = true;
    float soundVolume = 0.5f;

    int hotkeyAdd = VK_F6;   // д╛хо©Л╫щ╪Э
    int hotkeySub = VK_F5;

    counter_element color = { ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_Text)) };
};