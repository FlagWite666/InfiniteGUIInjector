#include "CpsItem.h"
#include "App.h"
bool GetKeyDown(int key)
{
    return GetAsyncKeyState(key) & 0x8000;
}

bool IsPressedOnce(int vk)
{
    if (vk <= 0)
        return false;
    static bool PrevKeyState[512] = { false };

    bool CurrKeyState = GetKeyDown(vk);
    bool IsClick = false;
    if (PrevKeyState[vk] != CurrKeyState)
    {
        IsClick = CurrKeyState;
        PrevKeyState[vk] = CurrKeyState;
    }
    return IsClick;
}

// 检查是否到了更新的时间
bool CpsItem::ShouldCpsUpdate() {
    auto now = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCpsTime).count();
    return elapsedTime >= cpsIntervalMs;
}

// 更新操作
void CpsItem::MarkUpCPSdated() {
    lastCpsTime = std::chrono::steady_clock::now();
}


void CpsItem::Update()
{
    if (ShouldCpsUpdate())
    {
        cps.processClick();
        MarkUpCPSdated();
    }
    if (App::Instance().clientHwnd != GetForegroundWindow())
        return;
    if (IsPressedOnce(VK_LBUTTON))
    {
        cps.AddLeftClick(20);
    }
    if (IsPressedOnce(VK_RBUTTON))
    {
        cps.AddRightClick(20);
    }
}

void CpsItem::DrawContent()
{
    

    bool needMiddleFix = showLeft && showRight;
    std::string middleFix = needMiddleFix ? " | " : "";
    std::string left = showLeft ? std::to_string(cps.GetLeftCps()) : "";
    std::string right = showRight ? std::to_string(cps.GetRightCps()) : "";

    std::string text = left + middleFix + right;
    ImGuiStd::TextShadow((prefix + text + suffix).c_str());
}

void CpsItem::DrawSettings()
{
    DrawModuleSettings();
    ImGui::Checkbox(u8"左键", &showLeft);
    ImGui::Checkbox(u8"右键", &showRight);

    if (ImGui::CollapsingHeader(u8"通用设置"))
    {
        DrawWindowSettings();
        DrawAffixSettings();
    }
}

void CpsItem::Load(const nlohmann::json& j)
{
    LoadItem(j);
    LoadAffix(j);
    LoadWindow(j);
    if (j.contains("showLeft")) showLeft = j["showLeft"];
    if (j.contains("showRight")) showRight = j["showRight"];
}

void CpsItem::Save(nlohmann::json& j) const
{
    SaveItem(j);
    SaveAffix(j);
    SaveWindow(j);
    j["showLeft"] = showLeft;
    j["showRight"] = showRight;
}
