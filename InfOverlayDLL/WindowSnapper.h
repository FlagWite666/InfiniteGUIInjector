#pragma once
#include "imgui/imgui.h"
#include <intrin.h>
#include <Windows.h>
#include <stdint.h>
#include <algorithm>

enum SnapState : uint32_t {
    SNAP_NONE = 0,

    // ±ß
    SNAP_LEFT = 1 << 0,
    SNAP_RIGHT = 1 << 1,
    SNAP_TOP = 1 << 2,
    SNAP_BOTTOM = 1 << 3,

    // ÖÐÐÄ
    SNAP_CENTER_X = 1 << 4,
    SNAP_CENTER_Y = 1 << 5,
};

struct SnapResult {
    ImVec2 snappedPos;
    SnapState snapState = SNAP_NONE;
};

class WindowSnapper
{
public:
    static SnapResult ComputeSnap(
        const ImVec2& pos,
        const ImVec2& size,
        float screenW,
        float screenH,
        float snapDist);

    static void DrawGuides(const SnapResult& r, float screenW, float screenH);
    static void KeepSnapped(ImVec2& pos, const ImVec2& size, float screenW, float screenH, const SnapState& r);
};