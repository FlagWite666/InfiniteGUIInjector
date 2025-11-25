#include "Menu.h"
#include "imgui/imgui.h"
#include "ImGuiStd.h"
#include "App.h"
#include "ConfigManager.h"
#include "GlobalConfig.h"
#include "FileUtils.h"
#include "StringConverter.h"
#include "TextItem.h"
#include "FileCountItem.h"
#include "BilibiliFansItem.h"
#include "CounterItem.h"
#include <thread>

void ShowFontSelection(GlobalConfig* globalConfig) {
    static std::vector<FileUtils::FontInfo> fontFiles;

    // Get font files if not loaded yet
    if (fontFiles.empty()) {
        fontFiles = FileUtils::GetFontsFromDirectory(L"C:\\Windows\\Fonts");
        ////获取用户名
        //std::wstring username = L"";
        //DWORD usernameSize = 0;

        // 获取用户名
        wchar_t username[256]; // 确保足够空间来存储用户名
        DWORD usernameSize = sizeof(username) / sizeof(username[0]);
        GetUserNameW(username, &usernameSize);
        std::vector<FileUtils::FontInfo> userFonts = FileUtils::GetFontsFromDirectory(L"C:\\Users\\" + std::wstring(username) + L"\\AppData\\Local\\Microsoft\\Windows\\Fonts");
        fontFiles.insert(fontFiles.end(), userFonts.begin(), userFonts.end());
    }

    if(ImGui::CollapsingHeader(u8"字体选择", ImGuiTreeNodeFlags_FramePadding))
    {
        ImGui::BeginGroup();
        if (ImGui::Selectable(u8"默认字体-Uranus_Pixel_11Px (重启生效)")) {
            // Load the selected font
            globalConfig->fontPath = "default";
        }

        for (size_t i = 0; i < fontFiles.size(); ++i) {
            if (ImGui::Selectable(fontFiles[i].name.c_str())) {
                // Load the selected font
                ImGuiIO& io = ImGui::GetIO();
                io.FontDefault = io.Fonts->AddFontFromFileTTF(StringConverter::WstringToUtf8(fontFiles[i].path).c_str(), 20.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
                globalConfig->fontPath = StringConverter::WstringToUtf8(fontFiles[i].path);
            }
        }
        ImGui::EndGroup();
    }

}

Menu::Menu()
{
}

void Menu::Render()
{
    if(!open)
        return;
    // 首次渲染时，记录圆角状态
    static int firstRender = 5;

    static bool isStyleEditorShow = false;
    if (isStyleEditorShow)
    {

        ImGui::ShowStyleEditor();
    }

    //使窗口显示在屏幕中间
    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetIO().DisplaySize.x / 2), (ImGui::GetIO().DisplaySize.y - ImGui::GetIO().DisplaySize.y / 2)), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(1000, 618), ImGuiCond_Once);

    ImGui::Begin(u8"主控制面板", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

    ImGui::Text(u8"-->主控制面板");

    ImGui::SameLine();

    if (ImGui::Button(u8"保存配置"))
        ConfigManager::Instance().Save(FileUtils::GetConfigPath());


    ImGui::SameLine();

    //将退出按钮显示在右上角
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 45);

    if (ImGui::Button("  X  "))
    {
        Toggle();
    }

    ImGui::SameLine();

    //将退出按钮显示在右上角
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 90);

    if (ImGui::Button("  +  "))
    {
        isStyleEditorShow = !isStyleEditorShow;
    }

    ImGui::Separator();

    //显示公告
    ImGui::BeginChild("Announce", ImVec2(0, 100), true);
    ImGuiStd::TextShadow(App::Instance().announcement.c_str());
    ImGui::EndChild();


    ImGui::Separator();
    if (ImGui::CollapsingHeader(u8"全局设置", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
    {
        if (ImGui::SliderFloat(u8"圆角半径", &GlobalConfig::Instance().roundCornerRadius, 0.0f, 10.0f, "%.1f"))
        {
            ImGuiStyle& style = ImGui::GetStyle();
            float roundCornerRadius = GlobalConfig::Instance().roundCornerRadius;
            style.WindowRounding = roundCornerRadius;
            style.FrameRounding = roundCornerRadius;
            style.GrabRounding = roundCornerRadius;
            style.ScrollbarRounding = roundCornerRadius;
            style.TabRounding = roundCornerRadius;
            style.ChildRounding = roundCornerRadius;
            style.PopupRounding = roundCornerRadius;
        }

        //设置主UI快捷键

        ImGuiStd::Keybind(u8"UI快捷键：", GlobalConfig::Instance().menuKey);

        ShowFontSelection(&GlobalConfig::Instance());

    }

    ImGui::Separator();

    // 添加各种信息项按钮

    if (ImGui::Button(u8"添加 文本")) {
        ItemManager::Instance().AddMulti(std::make_unique<TextItem>());
    }

    if (ImGui::Button(u8"添加 文件数量")) {
        ItemManager::Instance().AddMulti(std::make_unique<FileCountItem>());
    }

    if (ImGui::Button(u8"添加 粉丝数")) {
        ItemManager::Instance().AddMulti(std::make_unique<BilibiliFansItem>());
    }

    if (ImGui::Button(u8"添加 计数器"))
    {
        ItemManager::Instance().AddMulti(std::make_unique<CounterItem>());
    }

    ImGui::Separator();
    DrawItemList();

    ImGui::Separator();
    //显示关于
    if (ImGui::CollapsingHeader(u8"关于", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
    {
        ImGuiStd::TextShadow(u8"无限小窗InfOverlay");
        ImGui::SameLine();
        std::string appVersion = std::to_string(App::Instance().appVersion.major) + "." + std::to_string(App::Instance().appVersion.minor) + "." + std::to_string(App::Instance().appVersion.build);
        ImGuiStd::TextShadow(("v" + appVersion).c_str());
        ImGui::SameLine();
        static std::atomic<bool> checkingUpdate = false;   // 是否在检查
        static std::atomic<bool> updateFinished = false;   // 检查是否完成
        static bool updateHasNew = false;                  // 是否发现新版本
        static std::thread updateThread;                   // 工作线程
        // 点击按钮：开始异步检查
        if (ImGui::Button(u8"检查更新"))
        {
            // 打开“正在检查”窗口
            ImGui::OpenPopup(u8"-->检查更新...");

            checkingUpdate = true;
            updateFinished = false;

            // 启动后台线程
            updateThread = std::thread([]()
                {
                    bool result = App::Instance().CheckUpdate();
                    updateHasNew = !result;   // result=false => 有新版本
                    updateFinished = true;
                    checkingUpdate = false;
                });

            updateThread.detach();
        }
        if (ImGui::BeginPopupModal(u8"-->检查更新...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (checkingUpdate)
            {
                ImGuiStd::TextShadow(u8"正在检查更新，请稍候...");
            }
            else if (updateFinished)
            {
                if (updateHasNew)
                {
                    ImGuiStd::TextShadow(u8"发现新版本！");
                    std::string cloudVersion =
                        std::to_string(App::Instance().cloudVersion.major) + "." +
                        std::to_string(App::Instance().cloudVersion.minor) + "." +
                        std::to_string(App::Instance().cloudVersion.build);

                    ImGuiStd::TextShadow((u8"最新版本：" + cloudVersion).c_str());
                }
                else
                    ImGuiStd::TextShadow(u8"目前已是最新版本");
                if (ImGui::Button(u8"确定"))
                {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
        ImGuiStd::TextShadow(u8"作者：");
        ImGui::SameLine();
        if (ImGui::Button(App::Instance().appAuthor.c_str()))
        {
            ShellExecute(NULL, NULL, L"https://space.bilibili.com/399194206", NULL, NULL, SW_SHOWNORMAL);
        }
        ImGuiStd::TextShadow(u8"相关链接：");
        ImGui::SameLine();
        if (ImGui::Button(u8"爱发电"))
        {
            ShellExecute(NULL, NULL, L"https://ifdian.net/a/qc_max", NULL, NULL, SW_SHOWNORMAL);
        }
        ImGui::SameLine();
        ImGuiStd::TextShadow(u8" & ");

        ImGui::SameLine();
        if (ImGui::Button(u8"GitHub"))
        {
            ShellExecute(NULL, NULL, L"https://github.com/QCMaxcer/InfOverlay-DLL", NULL, NULL, SW_SHOWNORMAL);
        }
    }

    ImGui::End();
}

void Menu::DrawItemList()
{
    auto& items = ItemManager::Instance().GetAllItems();

    for (int i = 0; i < items.size(); i++)
    {
        Item* item = items[i];
        if (item->type == Hidden)
            continue;

        ImGui::Checkbox(("##" + std::to_string(i)).c_str(), &item->isEnabled);
        ImGui::SameLine();

        ImGui::Text("[%d] %s", i, item->name.c_str());
        ImGui::SameLine();
        ImGui::PushID(i);

        if (item->IsMultiInstance())
        {
            if (ImGui::Button(u8"删除"))
            {
                ItemManager::Instance().RemoveMulti(item);
                ImGui::PopID();
                break;
            }
            ImGui::SameLine();
        }
        if (ImGui::Button(u8"设置"))
        {
            ImGui::OpenPopup("ItemEditor");
        }

        if (ImGui::BeginPopup("ItemEditor"))
        {
            DrawItemEditor(item);
            ImGui::EndPopup();
        }

        ImGui::PopID();
    }
}

void Menu::DrawItemEditor(Item* item)
{
    ImGui::Text(u8"编辑项：%s", item->name.c_str());

    // 根据不同类型显示不同配置项
    item->DrawSettings();
}

void Menu::Toggle(bool open)
{
    this->open = open;
}

void Menu::Toggle()
{
    open = !open;
    if (!open)
    {
        RECT rect;
        // 获取窗口的矩形位置和大小
        if (GetWindowRect(App::Instance().clientHwnd, &rect)) {
            // 计算窗口中心的位置
            int centerX = (rect.left + rect.right) / 2;
            int centerY = (rect.top + rect.bottom) / 2;

            // 设置鼠标位置
            SetCursorPos(centerX, centerY);
        }
    }
}