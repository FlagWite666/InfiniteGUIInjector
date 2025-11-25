#pragma once
#include "Sprint.h"
#include "KeyState.h"
#include "GameStateDetector.h"
void Sprint::OnKeyEvent(bool state, bool isRepeat, WPARAM key)
{
    if (key == NULL) return;
    if (state && !isRepeat) //按键按下
    {
        if (key == keybinds.at(u8"激活键："))
        {
            isActivated = !isActivated;
        }
    }
}
void Sprint::Update()
{
    if (!isActivated) return;
    if (GetKeyDown(keybinds.at(u8"前进键：")) && GameStateDetector::Instance().IsInGame())
    {
        isWalking = true;
        lastIsWalking = true;
    }
    else
    {
        isWalking = false;
    }
    if (isWalking)
    {
        SetKeyDown(keybinds.at(u8"疾跑键："), 1);
    }
    if (!isWalking && lastIsWalking)
    {
        SetKeyUp(keybinds.at(u8"疾跑键："), 1);
        lastIsWalking = false;
    }


}

void Sprint::DrawContent()
{
    std::string text = isActivated? isWalking? u8"正在疾跑" : u8"停止疾跑" : u8"未激活";
    ImGuiStd::TextShadow((prefix + text + suffix).c_str());
}

void Sprint::DrawSettings()
{
    DrawModuleSettings();
    ImGui::Checkbox(u8"激活", &isActivated);
    ImGui::Checkbox(u8"显示窗口", &isWindowShow);

    DrawKeybindSettings();
    if (ImGui::CollapsingHeader(u8"通用设置"))
    {
        DrawWindowSettings();
        DrawAffixSettings();
    }
}

void Sprint::Load(const nlohmann::json& j)
{
    LoadItem(j);
    LoadAffix(j);
    LoadWindow(j);
    if (j.contains("isWindowShow")) isWindowShow = j["isWindowShow"];
    if (j.contains("isActivated")) isActivated = j["isActivated"];
}

void Sprint::Save(nlohmann::json& j) const
{
    SaveItem(j);
    SaveAffix(j);
    SaveWindow(j);
    j["isWindowShow"] = isWindowShow;
    j["isActivated"] = isActivated;
}
