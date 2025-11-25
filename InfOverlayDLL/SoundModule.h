#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "ImGuiStd.h"
class SoundModule
{
public:

    void DrawSoundSettings()
    {
        ImGui::Checkbox(u8"提示音", &isPlaySound);
        ImGui::SliderFloat(u8"音量", &soundVolume, 0.0f, 1.0f, "%.2f");
    }
protected:
    void LoadSound(const nlohmann::json& j)
    {
        if (j.contains("isPlaySound")) isPlaySound = j["isPlaySound"];
        if (j.contains("soundVolume")) soundVolume = j["soundVolume"];
    }
    void SaveSound(nlohmann::json& j) const
    {
        j["isPlaySound"] = isPlaySound;
        j["soundVolume"] = soundVolume;
    }
    bool isPlaySound;
    float soundVolume;    // 声音音量（0.0~1.0）
};