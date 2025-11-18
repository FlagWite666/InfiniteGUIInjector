#include "ConfigManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

bool ConfigManager::Save(const std::string& filePath,
    const GlobalConfig& global,
    const InfoManager& info)
{
    nlohmann::json j;
    global.Save(j["global"]);
    info.Save(j["info"]);

    std::ofstream f(filePath);
    if (!f.is_open()) return false;
    f << j.dump(4);
    return true;
}

bool ConfigManager::Load(const std::string& filePath,
    GlobalConfig& global,
    InfoManager& info)
{
    std::ifstream f(filePath);
    if (!f.is_open()) return false;

    nlohmann::json j;
    f >> j;

    if (j.contains("global")) global.Load(j["global"]);
    if (j.contains("info")) info.Load(j["info"]);

    return true;
}