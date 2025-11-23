#pragma once
#include <Windows.h>
#include "imgui\imgui.h"
#include "Item.h"
#include "WindowModule.h"
#include "UpdateModule.h"
#include "AffixModule.h"
#include <string>
#include <chrono>

struct click_container {
    std::vector<int> leftContainer;
    std::vector<int> rightContainer;

    void AddLeftClick(int times)
    {
        leftContainer.push_back(times);
    }

    void AddRightClick(int times)
    {
        rightContainer.push_back(times);
    }

    void processClick()
    {
        for (int i = 0; i < leftContainer.size(); i++)
        {
            leftContainer[i] -= 1;
            if (leftContainer[i] == 0)
            {
                leftContainer.erase(leftContainer.begin() + i);
            }
        }
        for (int i = 0; i < rightContainer.size(); i++)
        {
            rightContainer[i] -= 1;
            if (rightContainer[i] == 0)
            {
                rightContainer.erase(rightContainer.begin() + i);
            }
        }
    }

    int GetLeftCps()
    {
        int left = (int)leftContainer.size();
        return left;
    }
    int GetRightCps()
    {
        int right = (int)rightContainer.size();
        return right;
    }
};

class CpsItem : public Item, public WindowModule, public UpdateModule, public AffixModule
{
public:
    CpsItem() {
        type = Hud; // 信息项类型
        multiType = Singlton;    // 信息项是否可以多开
        name = u8"CPS显示";
        description = u8"显示左右键CPS";
        isEnabled = false;
        refreshIntervalMs = 5;
        lastUpdateTime = std::chrono::steady_clock::now();
        lastCpsTime = std::chrono::steady_clock::now();
        prefix = "[CPS: ";
        suffix = "]";
    }
    ~CpsItem() {}

    void Update() override;
    void DrawContent() override;
    void DrawSettings() override;
    void Load(const nlohmann::json& j) override;
    void Save(nlohmann::json& j) const override;
private:

    click_container cps;
    bool showLeft = true;
    bool showRight = true;
    // 检查是否到了更新的时间
    bool ShouldCpsUpdate();

    // 更新操作
    void MarkUpCPSdated();
    int cpsIntervalMs = 50;
    std::chrono::steady_clock::time_point lastCpsTime;  // 记录最后更新时间
};