#include "ConfigManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

bool ConfigManager::Save(const std::string& filePath)
{
    nlohmann::json j;
    GlobalConfig::Instance().Save(j["global"]);
    ItemManager::Instance().Save(j["item"]);

    std::ofstream f(filePath);
    if (!f.is_open()) return false;
    f << j.dump(4);
    return true;
}

bool ConfigManager::Load(const std::string& filePath)
{
    std::ifstream f(filePath);
    if (!f.is_open()) return false;

    nlohmann::json j;
    f >> j;
    if (j.contains("global")) GlobalConfig::Instance().Load(j["global"]);
    if (j.contains("item")) ItemManager::Instance().Load(j["item"]);
    return true;
}