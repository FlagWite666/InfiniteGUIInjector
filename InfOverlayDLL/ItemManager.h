#pragma once
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "Item.h"
#include "GlobalConfig.h"

class ItemManager {
public:
    ItemManager();

    static ItemManager& Instance() {
        static ItemManager instance;
        return instance;
    }

    void AddItem(std::shared_ptr<Item> item);
    void RemoveItem(int index);

    void UpdateAll();

    void RenderAll(HWND hwnd);

    void ProcessKeyEvents(WPARAM wParam);

    void Load(const nlohmann::json& j);
    void Save(nlohmann::json& j) const;



    std::vector<std::shared_ptr<Item>>& GetItems() { return items; }

private:
    std::vector<std::shared_ptr<Item>> items;
};