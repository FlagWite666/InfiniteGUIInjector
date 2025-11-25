#pragma once
#include <string>
#include "imgui/imgui.h"
#include <nlohmann/json.hpp>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include "GlobalConfig.h"
#include "WindowSnapper.h"
#include "App.h"
static const float SNAP_DISTANCE = 15.0f;
static bool isSnapping = false;

class WindowModule
{
public:
    virtual void DrawContent() = 0;       // 绘制内容（文本、图形等）

    void DrawWindowSettings()
    {
        ImGui::Checkbox(u8"固定", &clickThrough);

        ImGui::Checkbox(u8"显示边框", &showBorder);


        ImGui::SliderFloat(u8"背景透明度", &alpha, 0.0f, 1.0f, "%.1f");

        ImGui::Checkbox(u8"自定义窗口大小", &isCustomSize);
        if (isCustomSize) {
            ImGui::InputFloat(u8"宽度", &width, 1.0f, 1.0f, "%.1f");
            ImGui::InputFloat(u8"高度", &height, 1.0f, 1.0f, "%.1f");
        }
        ImGui::InputFloat(u8"窗口 X", &x, 1.0f, 1.0f, "%.1f");
        ImGui::InputFloat(u8"窗口 Y", &y, 1.0f, 1.0f, "%.1f");

        ImGui::InputFloat(u8"字体大小", &fontSize, 1.0f, 1.0f, "%.1f");
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

        ImGui::SetNextWindowBgAlpha(alpha); // 半透明
        if (!showBorder) ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // 边框透明

        ImGuiWindowFlags flags = 0;
        //if (!allowResize) flags |= ImGuiWindowFlags_NoResize;
        //if (!allowMove)   flags |= ImGuiWindowFlags_NoMove;
        if (!isCustomSize) flags |= ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoTitleBar;
        flags |= ImGuiWindowFlags_NoScrollbar;
        flags |= ImGuiWindowFlags_NoSavedSettings;


        if (clickThrough) flags |= ImGuiWindowFlags_NoInputs;

        ImGui::Begin(GetActualWindowName().c_str(), nullptr, flags);

        ImGui::SetWindowFontScale(fontSize / 20.0f);

        isMoving = ImGui::IsMouseDragging(0, 1.0f) && ImGui::IsWindowFocused();

        isHovered = ImGui::IsWindowHovered();

        DrawContent();

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

            // 获取 screenW,screenH（DLL 中已经有 g_hwnd）
            RECT rc;
            GetClientRect(g_hwnd, &rc);
            float sw = (float)rc.right;
            float sh = (float)rc.bottom;
            WindowSnapper::KeepSnapped(pos, sz, sw, sh, snapState);
            // 设置吸附后的位置
            ImGui::SetWindowPos(pos, ImGuiCond_Always);

            // 保存到 Item
            x = pos.x;
            y = pos.y;
        }

        ImGui::End();
        if (!showBorder) ImGui::PopStyleColor(); // 边框透明
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

            // 获取 screenW,screenH（DLL 中已经有 g_hwnd）
            RECT rc;
            GetClientRect(g_hwnd, &rc);
            float sw = (float)rc.right;
            float sh = (float)rc.bottom;

            SnapResult snap;
            if (isSnapping)
            {
                // 计算吸附
                snap = WindowSnapper::ComputeSnap(pos, sz, sw, sh, SNAP_DISTANCE);
                // 画吸附线
                WindowSnapper::DrawGuides(snap, sw, sh);
            }
            else
                snap = WindowSnapper::ComputeSnap(pos, sz, sw, sh, 0.0f);

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

        if (j.contains("alpha")) alpha = j["alpha"];
        if (j.contains("showBorder")) showBorder = j["showBorder"];
        if (j.contains("fontSize")) fontSize = j["fontSize"];
        if (j.contains("clickThrough")) clickThrough = j["clickThrough"];

        if (j.contains("snapState")) snapState = j["snapState"];

    }
    void SaveWindow(nlohmann::json& j) const
    {
        j["isCustomSize"] = isCustomSize;
        j["x"] = x;
        j["y"] = y;
        j["width"] = width;
        j["height"] = height;

        j["alpha"] = alpha;
        j["showBorder"] = showBorder;
        j["fontSize"] = fontSize;
        j["clickThrough"] = clickThrough;

        j["snapState"] = snapState;
    }

    bool isWindowShow = true;

    bool isCustomSize = false;    //是否自定义大小
    float x = 100.0f;
    float y = 40.0f;

    SnapState snapState = SNAP_NONE;
    float width = 250.0f;
    float height = 80.0f;

    float alpha = 1.0f;              // 透明度
    bool showBorder = true;        // 开启边框
    float fontSize = 20.0f;               // 字体大小


    bool allowMove = true;
    bool allowResize = true;
    bool clickThrough = false;

    bool isMoving = false;
    bool isHovered = true;


};