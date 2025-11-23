#include "ItemManager.h"
#include "TimeItem.h"
#include "FpsItem.h"
#include "BilibiliFansItem.h"
#include "FileCountItem.h"
#include "CounterItem.h"
#include "DanmakuItem.h"
#include "TextItem.h"
#include "KeystrokesItem.h"
#include "CpsItem.h"
#include "GlobalConfig.h"

#include <chrono>


ItemManager::ItemManager()
{
    // 注册默认信息项
    AddItem(std::make_shared<TimeItem>());
    AddItem(std::make_shared<FpsItem>());
    AddItem(std::make_shared<DanmakuItem>());
    AddItem(std::make_shared<KeystrokesItem>());
    AddItem(std::make_shared<CpsItem>());
}

void ItemManager::AddItem(std::shared_ptr<Item> item)
{
    items.push_back(std::move(item));
}


// ---------------------------------------------
// 删除信息项（简单版）
// ---------------------------------------------
void ItemManager::RemoveItem(int index)
{
    if (index >= 0 && index < items.size()) {
        items.erase(items.begin() + index);

    }

}

void ItemManager::UpdateAll()
{
    auto now = std::chrono::steady_clock::now();

    for (auto& item : items)
    {
        if (!item->isEnabled) continue; // 跳过禁用的模块
        if (auto upd = dynamic_cast<UpdateModule*>(item.get()))
        {
            if (upd->ShouldUpdate())
            {
                upd->Update();
                upd->MarkUpdated();
            }
                
        }
    }
}

void ItemManager::ProcessKeyEvents(UINT key, WPARAM wParam, LPARAM lParam) //key：按键，wParam：按下或释放标志，lParam：是否是重复按键
{
    for (auto& item : items)
    {
        if (!item->isEnabled) continue; // 跳过禁用的模块
        if (auto kbd = dynamic_cast<KeybindModule*>(item.get()))
        {
            kbd->OnKeyEvent(key, wParam, lParam);
        }
    }
}

// ---------------------------------------------
// 渲染所有信息项（每帧调用）
// ---------------------------------------------
void ItemManager::RenderAll(HWND hwnd)
{
    for (auto& item : items) {
        if (!item->isEnabled) continue; // 跳过禁用的模块
        if (auto win = dynamic_cast<WindowModule*>(item.get()))
        {
            win->RenderWindow(hwnd);
        }
    }
}

// ---------------------------------------------
// 从 JSON 加载全部信息项
// ---------------------------------------------
void ItemManager::Load(const nlohmann::json& j)
{
    // 清理现有 MultiInstance 但保留 Singleton 已初始化的
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [](const std::shared_ptr<Item>& it) {
                return it->IsMultiInstance();
            }),
        items.end()
    );

    // 加载 SingletonItems（此时 Singleton 实例已存在，只需要加载配置）
    if (j.contains("SingletonItems"))
    {
        for (auto& node : j["SingletonItems"])
        {
            std::string type = node["type"].get<std::string>();

            for (auto& item : items)
            {
                if (!item->IsMultiInstance() && item->name == type) // 只加载到已存在 Singleton
                {
                    item->Load(node);
                    break;
                }
            }
        }
    }

    //  加载 MultiInstanceItems（每个都创建新实例）
    if (j.contains("MultiInstanceItems"))
    {
        for (auto& node : j["MultiInstanceItems"])
        {
            std::unique_ptr<Item> item;
            std::string type = node["type"].get<std::string>();

            if (type == u8"粉丝数显示")    item = std::make_unique<BilibiliFansItem>();
            if (type == u8"文件数量显示")       item = std::make_unique<FileCountItem>();
            if (type == u8"计数器")          item = std::make_unique<CounterItem>();
            if (type == u8"文本显示")             item = std::make_unique<TextItem>();
            if (!item) continue;

            item->Load(node);
            AddItem(std::move(item));
        }
    }
}

// ---------------------------------------------
// 保存所有信息项
// ---------------------------------------------
void ItemManager::Save(nlohmann::json& j) const
{
    j["SingletonItems"] = nlohmann::json::array();
    j["MultiInstanceItems"] = nlohmann::json::array();

    for (auto& item : items)
    {
        nlohmann::json node;
        item->Save(node);

        if (item->IsMultiInstance())
            j["MultiInstanceItems"].push_back(node);
        else
            j["SingletonItems"].push_back(node);
    }
}