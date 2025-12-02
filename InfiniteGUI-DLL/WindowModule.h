#pragma once
#include "WindowStyleModule.h"
#include "GlobalWindowStyle.h"
#include <string>
#include "imgui/imgui.h"
#include "ImGuiStd.h"
#include <nlohmann/json.hpp>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include "GlobalConfig.h"
#include "WindowSnapper.h"
#include "App.h"
#include "opengl_hook.h"
static const float SNAP_DISTANCE = 15.0f;
static bool isSnapping = false;

class WindowModule : public WindowStyleModule
{
public:

    WindowModule()
    {
        SetStyle();
    }
    virtual void DrawContent() = 0;       // 绘制内容（文本、图形等）

    void SetStyle()
    {
        if (isCustomStyle)
        {
            itemStylePtr = &itemStyle;

        }
        else
        {
            itemStylePtr = &GlobalWindowStyle::Instance().GetGlobeStyle();
        }
    }
    void DrawWindowSettings()
    {
        ImGui::Checkbox(u8"固定", &clickThrough);

        ImGui::Checkbox(u8"自定义窗口大小", &isCustomSize);
        if (isCustomSize) {
            ImGui::InputFloat(u8"宽度", &width, 1.0f, 1.0f, "%.1f");
            ImGui::InputFloat(u8"高度", &height, 1.0f, 1.0f, "%.1f");
        }
        ImGui::InputFloat(u8"窗口 X", &x, 1.0f, 1.0f, "%.1f");
        ImGui::InputFloat(u8"窗口 Y", &y, 1.0f, 1.0f, "%.1f");

        if (ImGui::Checkbox(u8"自定义窗口样式", &isCustomStyle))
        {
            SetStyle();
        }

        if (isCustomStyle) 
            DrawStyleSettings();
    }

    // ---------------------------
//   渲染整个窗口（统一逻辑）
// ---------------------------
    virtual void RenderWindow()
    {
        if (!isWindowShow) return;
        if (!isCustomSize)
            //ImGui::SetNextWindowSizeConstraints(ImVec2(10, 10), ImVec2(2560, 1440));
            ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);

        if (!isMoving)
        {
            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
            if (isCustomSize)
                ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
        }
        HWND g_hwnd = App::Instance().clientHwnd;


        ImGui::PushStyleColor(ImGuiCol_WindowBg, itemStylePtr->bgColor); // 背景透明
        ImGui::PushStyleColor(ImGuiCol_Border, itemStylePtr->borderColor); // 边框透明
        ImGui::PushStyleColor(ImGuiCol_Text, itemStylePtr->fontColor); // 字体颜色

        ImGuiWindowFlags flags = 0;
        //if (!allowResize) flags |= ImGuiWindowFlags_NoResize;
        //if (!allowMove)   flags |= ImGuiWindowFlags_NoMove;
        if (!isCustomSize) flags |= ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoTitleBar;
        flags |= ImGuiWindowFlags_NoScrollbar;
        flags |= ImGuiWindowFlags_NoSavedSettings;


        if (clickThrough) flags |= ImGuiWindowFlags_NoInputs;

        ImGui::Begin(GetActualWindowName().c_str(), nullptr, flags);

        isMoving = ImGui::IsMouseDragging(0, 1.0f) && ImGui::IsWindowFocused();

        isHovered = ImGui::IsWindowHovered();

        ImGui::PushFont(NULL, itemStylePtr->fontSize);
        DrawContent();
        ImGui::PopFont();

        // 判断拖动/修改大小事件
        if (isMoving)
        {
            HandleDrag(g_hwnd);
        }
        else
        {
            // 当前窗口位置大小
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 sz = ImGui::GetWindowSize();

            WindowSnapper::KeepSnapped(pos, sz, (float)opengl_hook::screen_size.x, (float)opengl_hook::screen_size.y, snapState);
            // 设置吸附后的位置
            ImGui::SetWindowPos(pos, ImGuiCond_Always);

            // 保存到 Item
            x = pos.x;
            y = pos.y;
        }

        ImGui::End();
        ImGui::PopStyleColor(3);
    }
private:
        std::string GetActualWindowName() const {
            return "##" + std::to_string((uintptr_t)this);
        }

        void HandleDrag(HWND g_hwnd)
        {
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
                isSnapping = true;
            else
                isSnapping = false;

            // 当前窗口位置大小
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 sz = ImGui::GetWindowSize();

            SnapResult snap;
            if (isSnapping)
            {
                // 计算吸附
                snap = WindowSnapper::ComputeSnap(pos, sz, (float)opengl_hook::screen_size.x, (float)opengl_hook::screen_size.y, SNAP_DISTANCE);
                // 画吸附线
                WindowSnapper::DrawGuides(snap, (float)opengl_hook::screen_size.x, (float)opengl_hook::screen_size.y);
            }
            else
                snap = WindowSnapper::ComputeSnap(pos, sz, (float)opengl_hook::screen_size.x, (float)opengl_hook::screen_size.y, 0.0f);

            // 设置吸附后的位置
            ImGui::SetWindowPos(snap.snappedPos, ImGuiCond_Always);

            // 保存到 Item
            x = snap.snappedPos.x;
            y = snap.snappedPos.y;
            snapState = snap.snapState;

            //保存窗口大小
            width = ImGui::GetWindowSize().x;
            height = ImGui::GetWindowSize().y;
        }

protected:
    void LoadWindow(const nlohmann::json& j)
    {
        if (j.contains("isCustomSize")) isCustomSize = j["isCustomSize"];
        if (j.contains("x")) x = j["x"];
        if (j.contains("y")) y = j["y"];
        if (j.contains("width")) width = j["width"];
        if (j.contains("height")) height = j["height"];

        if (j.contains("clickThrough")) clickThrough = j["clickThrough"];

        if (j.contains("snapState")) snapState = j["snapState"];

        if (j.contains("isCustomStyle")) isCustomStyle = j["isCustomStyle"];
        SetStyle();
        LoadStyle(j);
    }
    void SaveWindow(nlohmann::json& j) const
    {
        j["isCustomSize"] = isCustomSize;
        j["x"] = x;
        j["y"] = y;
        j["width"] = width;
        j["height"] = height;

        j["clickThrough"] = clickThrough;

        j["snapState"] = snapState;

        j["isCustomStyle"] = isCustomStyle;

        SaveStyle(j);
    }

    bool isWindowShow = true;

    bool isCustomSize = false;    //是否自定义大小
    float x = 100.0f;
    float y = 40.0f;

    SnapState snapState = SNAP_NONE;
    float width = 250.0f;
    float height = 80.0f;

    bool allowMove = true;
    bool allowResize = true;
    bool clickThrough = false;

    bool isMoving = false;
    bool isHovered = true;

    bool isCustomStyle = false;

    ItemStyle* itemStylePtr = &itemStyle;
};