#pragma once
#include "Item.h"
#include "AffixModule.h"
#include "SoundModule.h"
#include "UpdateModule.h"
#include "WindowModule.h"
#include <string>


struct bilibili_fans_element {
    ImVec4 color;
};

class BilibiliFansItem : public Item, public AffixModule, public SoundModule, public UpdateModule, public WindowModule {
public:
    BilibiliFansItem() {
        type = Hud; // 信息项类型
        multiType = MultiInstance;    // 信息项是否可以多开
        name = u8"粉丝数显示";
        description = u8"显示B站用户的粉丝数";
        isPlaySound = true;    // 是否播放声音
        soundVolume = 0.5f;    // 声音音量（0.0~1.0）
        refreshIntervalMs = 3000;
        lastUpdateTime = std::chrono::steady_clock::now();
        prefix = u8"[粉丝数:";
        suffix = "]";
    }

    void Update() override;
    void DrawContent() override;
    void DrawSettings() override;
    void Load(const nlohmann::json& j) override;
    void Save(nlohmann::json& j) const override;

private:
    long long uid = 399194206;          // B站用户UID
    int fansCount = 0;    // 粉丝数（从API解析）
    int lastFansCount = 0;    // 粉丝数（从API解析）

    bilibili_fans_element color = { ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_Text)) };
};