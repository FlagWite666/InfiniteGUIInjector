#include "CounterItem.h"
#include "AudioManager.h"
#include "imgui\imgui.h"
#include "ImGuiStd.h"
#include "imgui\imgui_internal.h"
#include <string>
#include <windows.h>



void CounterItem::Update()
{
    // 加法键按下
    if (GetAsyncKeyState(hotkeyAdd) & 1)   // 1 表示刚按下
        count++;

    // 减法键按下
    if (GetAsyncKeyState(hotkeySub) & 1)
        count--;
}

void CounterItem::DrawContent()
{
    if (count > lastCount)
    {
        color.color = ImVec4(0.1f, 1.0f, 0.1f, 1.0f); //绿色
        lastCount = count;
        if (isPlaySound) AudioManager::Instance().playSound("counter\\counter_up.wav", soundVolume);
    }
    else if (count < lastCount)
    {
        color.color = ImVec4(1.0f, 0.1f, 0.1f, 1.0f); //红色
        lastCount = count;
        if (isPlaySound) AudioManager::Instance().playSound("counter\\counter_down.wav", soundVolume);
    }

    ImVec4 targetTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

    //获取io
    ImGuiIO& io = ImGui::GetIO();

    //计算速度
    float speed = 3.0f * io.DeltaTime;
    color.color = ImLerp(color.color, targetTextColor, speed);

    ImGuiStd::TextColoredShadow(color.color, (prefix + std::to_string(count) + suffix).c_str());

}

void CounterItem::Load(const nlohmann::json& j)
{
    if (j.contains("count")) count = j["count"];
    lastCount = count;
    if (j.contains("hotkeyAdd")) hotkeyAdd = j["hotkeyAdd"];
    if (j.contains("hotkeySub")) hotkeySub = j["hotkeySub"];

    if (j.contains("isPlaySound")) isPlaySound = j["isPlaySound"];
    if (j.contains("soundVolume")) soundVolume = j["soundVolume"];
    LoadInfoItemConfig(j);
}

void CounterItem::Save(nlohmann::json& j) const
{
    j["type"] = "counter";
    j["count"] = count;
    j["hotkeyAdd"] = hotkeyAdd;
    j["hotkeySub"] = hotkeySub;
    j["isPlaySound"] = isPlaySound;
    j["soundVolume"] = soundVolume;
    SaveInfoItemConfig(j);
}