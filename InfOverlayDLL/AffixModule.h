#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "ImGuiStd.h"
class AffixModule
{
public:

    void DrawAffixSettings()
    {
        ImGuiStd::InputTextStd(u8"Ç°×º", prefix);
        ImGuiStd::InputTextStd(u8"ºó×º", suffix);
    }
protected:
    void LoadAffix(const nlohmann::json& j)
    {
        if (j.contains("prefix")) prefix = j["prefix"];
        if (j.contains("suffix")) suffix = j["suffix"];
    }
    void SaveAffix(nlohmann::json& j) const
    {
        j["prefix"] = prefix;
        j["suffix"] = suffix;
    }

	std::string prefix = "[";
	std::string suffix = "]";
};