#include "WindowSnapper.h"
#include <cmath>

SnapResult WindowSnapper::ComputeSnap(
    const ImVec2& pos,
    const ImVec2& size,
    float screenW,
    float screenH,
    float snapDist)
{
    SnapResult r;
    SnapState state = SNAP_NONE;
    r.snappedPos = pos;

    float right = pos.x + size.x;
    float bottom = pos.y + size.y;

    float cx = pos.x + size.x * 0.5f;
    float cy = pos.y + size.y * 0.5f;

    // 边缘吸附
    if (fabs(pos.x) < snapDist) {
        r.snappedPos.x = 0;
        state = static_cast<SnapState>(state | SNAP_LEFT);
    }
    if (fabs(right - screenW) < snapDist) {
        r.snappedPos.x = screenW - size.x;
        state = static_cast<SnapState>(state | SNAP_RIGHT);
    }
    if (fabs(pos.y) < snapDist) {
        r.snappedPos.y = 0;
        state = static_cast<SnapState>(state | SNAP_TOP);
    }
    if (fabs(bottom - screenH) < snapDist) {
        r.snappedPos.y = screenH - size.y;
        state = static_cast<SnapState>(state | SNAP_BOTTOM);
    }

    // 中心吸附
    if (fabs(cx - screenW * 0.5f) < snapDist) {
        r.snappedPos.x = screenW * 0.5f - size.x * 0.5f;
        state = static_cast<SnapState>(state | SNAP_CENTER_X);
    }
    if (fabs(cy - screenH * 0.5f) < snapDist) {
        r.snappedPos.y = screenH * 0.5f - size.y * 0.5f;
        state = static_cast<SnapState>(state | SNAP_CENTER_Y);
    }

    r.snapState = state;

    return r;
}

void WindowSnapper::KeepSnapped(ImVec2& pos, const ImVec2& size, float screenW, float screenH, const SnapState& r)
{
    //保持边缘吸附
    if (r & SNAP_LEFT) {
        pos.x = 0;
    }
    if (r & SNAP_RIGHT) {
        pos.x = screenW - size.x;
    }
    if (r & SNAP_TOP) {
        pos.y = 0;
    }
    if (r & SNAP_BOTTOM) {
        pos.y = screenH - size.y;
    }

    //保持中心吸附
    if (r & SNAP_CENTER_X) {
        pos.x = screenW * 0.5f - size.x * 0.5f;
    }
    if (r & SNAP_CENTER_Y) {
        pos.y = screenH * 0.5f - size.y * 0.5f;
    }
}

void WindowSnapper::DrawGuides(const SnapResult& r, float screenW, float screenH)
{
    auto draw = ImGui::GetForegroundDrawList();
    ImU32 color = IM_COL32(120, 180, 255, 150);

    if (r.snapState & SNAP_LEFT) {
        draw->AddLine(ImVec2(0, 0), ImVec2(0, screenH), color, 2.0f);
    }

    if (r.snapState & SNAP_RIGHT) {
        draw->AddLine(ImVec2(screenW, 0), ImVec2(screenW, screenH), color, 2.0f);
    }

    if (r.snapState & SNAP_TOP) {
        draw->AddLine(ImVec2(0, 0), ImVec2(screenW, 0), color, 2.0f);
    }

    if (r.snapState & SNAP_BOTTOM) {
        draw->AddLine(ImVec2(0, screenH), ImVec2(screenW, screenH), color, 2.0f);
    }

    if (r.snapState & SNAP_CENTER_X) {
        draw->AddLine(ImVec2(screenW * 0.5f, 0), ImVec2(screenW * 0.5f, screenH), color, 2.0f);
    }

    if (r.snapState & SNAP_CENTER_Y) {
        draw->AddLine(ImVec2(0, screenH * 0.5f), ImVec2(screenW, screenH * 0.5f), color, 2.0f);
    }
}