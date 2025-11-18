#pragma once
#include "InfoItem.h"
#include <string>

struct file_count_element {
    ImVec4 color;
};

class FileCountItem : public InfoItem {
public:
    FileCountItem() {
        windowTitle = "File Count";
        refreshIntervalMs = 3000;   // 默认3秒刷新
    }

    void Update() override;
    void DrawContent() override;
    void Load(const nlohmann::json& j) override;
    void Save(nlohmann::json& j) const override;

private:
    size_t fileCount = 0;
    size_t lastFileCount = 0;

public:
    // 配置项
    std::string folderPath = "";   // 默认路径
    std::string errorMessage = "";
    bool recursive = false;           // 是否递归扫描
    std::string extensionFilter = ""; // 例如 ".txt" 为空则全部文件

    bool isPlaySound = true;
    float soundVolume = 0.5f;

    file_count_element color = { ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_Text)) };
};