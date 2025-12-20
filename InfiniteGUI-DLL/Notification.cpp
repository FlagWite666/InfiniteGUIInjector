#pragma once

#include "Notification.h"
#include "Anim.h"

void Notification::RenderGui()
{
    SetMaxElementPos();
    //获取io
    ImGuiIO& io = ImGui::GetIO();
    float speed = 5.0f * 3.0f * std::clamp(io.DeltaTime, 0.0f, 0.05f);
    if(state != Leaving)
    {
        curElement.pos = ImLerp(curElement.pos, targetElement.pos, speed);
        // 判断动画是否结束
        if (Anim::AlmostEqual(curElement.pos, targetElement.pos, 1.0f))
        {
            curElement.pos = targetElement.pos;
            state = Staying;
        }
    }
    else
    {
        curElement.pos = ImLerp(curElement.pos, startElement.pos, speed);
        // 判断动画是否结束
        if (Anim::AlmostEqual(curElement.pos, startElement.pos, 1.0f))
        {
            done = true;
        }
    }
    ImGui::SetNextWindowPos(curElement.pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    bool open = true;
    std::string windowName = "Notification" + std::to_string(id);
    PushRounding(itemStyle.windowRounding);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, itemStyle.bgColor);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, itemStyle.bgColor);
    ImGui::PushStyleColor(ImGuiCol_Border, itemStyle.borderColor);
    if (itemStyle.rainbowFont) processRainbowFont();
    else ImGui::PushStyleColor(ImGuiCol_Text, itemStyle.fontColor);
    ImGui::Begin(windowName.c_str(), &open, flags);

    ImGui::PushFont(opengl_hook::gui.iconFont, itemStyle.fontSize * 0.8f);
    ImGuiStd::TextShadow(icon.c_str());
    ImGui::PopFont();
    ImGui::PushFont(NULL, itemStyle.fontSize * 0.8f);
    ImGui::SameLine();
    float startX = ImGui::GetCursorPosX();
    ImGui::BeginDisabled();
    ImGuiStd::TextShadow(title.c_str());
    ImGui::EndDisabled();
    ImGui::PopFont();

    ImGui::Separator();
    ImGui::SetCursorPosX(startX);
    ImGui::PushFont(NULL, itemStyle.fontSize);
    ImGuiStd::TextShadow(message.c_str());
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(7);
}

void Notification::RenderBeforeGui()
{
}

void Notification::RenderAfterGui()
{
}

bool Notification::Done() const
{
    return done;
}

void Notification::SetMaxElementPos()
{
    ImVec2 maxScreenSize = ImVec2(opengl_hook::screen_size.x, opengl_hook::screen_size.y);
    startElement.pos = ImVec2(maxScreenSize.x + windowPadding, maxScreenSize.y - (placeIndex + 1.5f) * (size.y + windowPadding));
    targetElement.pos = ImVec2(maxScreenSize.x - size.x - windowPadding, startElement.pos.y);
}

void Notification::SetPlaceIndex(int index)
{
    this->placeIndex = index;
}

bool Notification::ShouldLeave()
{        //if (updateIntervalMs == -1) return false;
    auto now = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - joinTime).count();
    if(elapsedTime >= intervalMs)
    {
        state = Leaving;
        return true;
    }
    return false;
}
