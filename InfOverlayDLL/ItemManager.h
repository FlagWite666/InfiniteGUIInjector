#pragma once
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "Item.h"

class ItemManager {
public:
    ItemManager();
    static ItemManager& Instance() {
        static ItemManager instance;
        return instance;
    }

    void AddSingleton(Item* item);
    void AddMulti(std::unique_ptr<Item> item);
    void RemoveMulti(Item* target);

    void UpdateAll();
    void RenderAll();
    void ProcessKeyEvents(bool state, bool isRepeat, WPARAM key);

    void Load(const nlohmann::json& j);
    void Save(nlohmann::json& j) const;

    const std::vector<Item*>& GetAllItems() const { return allItems; }

private:
    std::vector<Item*> allItems;                      // 全部 item（指针，不析构）
    std::vector<Item*> singletonItems;                // 单例指针，不析构
    std::vector<std::unique_ptr<Item>> multiItems;    // 多例，由 manager 管理
};