#pragma once
#include <chrono>
#include <nlohmann/json.hpp>
#include "ImGuiStd.h"
class UpdateModule
{
public:
	virtual void Update() = 0;

    // 检查是否到了更新的时间
    bool ShouldUpdate() {
        //if (refreshIntervalMs == -1) return false;
        auto now = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count();
        return elapsedTime >= refreshIntervalMs;
    }

    // 更新操作
    void MarkUpdated() {
        lastUpdateTime = std::chrono::steady_clock::now();
    }
protected:
	int refreshIntervalMs;    // 默认 1 秒
	std::chrono::steady_clock::time_point lastUpdateTime = std::chrono::steady_clock::now();  // 记录最后更新时间
};