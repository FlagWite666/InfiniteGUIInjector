#include "CPSDetector.h"
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
bool CPSDetector::ShouldCpsUpdate() {
    auto now = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCpsTime).count();
    return elapsedTime >= cpsIntervalMs;
}

// 更新操作
void CPSDetector::MarkUpCPSdated() {
    lastCpsTime = std::chrono::steady_clock::now();
}


void CPSDetector::Update()
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

int CPSDetector::GetLeftCPS()
{
    return cps.GetLeftCPS();
}
int CPSDetector::GetRightCPS()
{
    return cps.GetRightCPS();
}


void CPSDetector::DrawSettings()
{
}

void CPSDetector::Load(const nlohmann::json& j)
{
    LoadItem(j);
}

void CPSDetector::Save(nlohmann::json& j) const
{
    SaveItem(j);
}
