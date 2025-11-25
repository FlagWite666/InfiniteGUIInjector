#pragma once
#include <Windows.h>

#include "imgui\imgui.h"
#include "Item.h"
#include "WindowModule.h"
#include "AffixModule.h"
#include "UpdateModule.h"
#include "KeybindModule.h"
#include <string>
#include <chrono>

class Sprint : public WindowModule, public UpdateModule, public KeybindModule, public AffixModule, public Item
{
public:
    Sprint() {

        type = Util; // 信息项类型
        multiType = Singleton;    // 信息项是否可以多开
        isEnabled = false; // 是否启用
        name = u8"强制疾跑";
        description = u8"强制疾跑";

        keybinds.insert(std::make_pair(u8"激活键：", 'I'));
        keybinds.insert(std::make_pair(u8"前进键：", 'W'));
        keybinds.insert(std::make_pair(u8"疾跑键：", VK_CONTROL));

        prefix = u8"[";
        suffix = "]";

        refreshIntervalMs = 5;
        lastUpdateTime = std::chrono::steady_clock::now();
    }

    static Sprint& Instance() {
        static Sprint instance;
        return instance;
    }

    void OnKeyEvent(bool state, bool isRepeat, WPARAM key) override;
    void Update() override;
    void DrawContent() override;
    void DrawSettings() override;
    void Load(const nlohmann::json& j) override;
    void Save(nlohmann::json& j) const override;

private:
    bool isActivated = false;
    bool isWalking = false;
    bool lastIsWalking = false;
};