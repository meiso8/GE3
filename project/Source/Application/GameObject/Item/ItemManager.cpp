#include "ItemManager.h"
#include"AABB.h"
#include"Line.h"
#include"Vector3.h"
#include "CollisionManager.h"
#include"InputBind.h"
#include"DebugUI.h"
#include"Player/RaySprite.h"
#include"SoundManager/SoundManager.h"
ItemManager::ItemManager()
{

}

void ItemManager::GenerateItems(const std::vector<std::string>& itemNames)
{
    items_.clear(); // 既存のアイテムをクリア

    for (const auto& name : itemNames) {
        std::shared_ptr<Item> item = nullptr;

        if (name == "CrowbarItem") {
            item = std::make_shared<CrowbarItem>();
        } else if (name == "SunMedal") {
            item = std::make_shared<SunMedal>();
        } else if (name == "GoldHeart") {
            item = std::make_shared<GoldHeart>();
        } else if (name == "SunRod") {
            item = std::make_shared<SunRod>();
        }

        if (item) {
            item->Init();
            items_[name] = item;
        }
    }

    itemSlot_.Init();
}

void ItemManager::Init() {
    items_.clear(); // 既存のアイテムをクリア
}

void ItemManager::Update() {
    for (auto& [name, item] : items_) {
        item->Update();
    }
    itemSlot_.Update();
}

void ItemManager::DrawGetItem()
{
    itemSlot_.Draw();
}

void ItemManager::Draw(Camera& camera) {
  
    for (auto& [name, item] : items_) {
        item->Draw(camera);
    }

}

void ItemManager::DrawUI() {

    itemSlot_.DrawUI();
}

bool ItemManager::HasItem(const std::string& name) {
    auto item = GetItem(name);
    return item && item->isGet_;
}

std::shared_ptr<Item> ItemManager::GetItem(const std::string& name) {
    auto it = items_.find(name);
    if (it != items_.end()) {
        return it->second;
    }
    return nullptr;
}

bool ItemManager::AddItemToSlot(const std::string& name) {
    auto item = GetItem(name);
    if (item) {
        itemSlot_.OnTriggerItemPickup(item);
        return true;
    }
    return false;
}

std::shared_ptr<Item> ItemManager::RaycastHitItem(RaySprite& raySprite) {


    for (auto& [name, item] : items_) {
        if (!item) continue;

        const auto& obj = item->object_;
        if (!obj) continue;

        AABB box = GetAABBWorldPos(item.get()); // AABBなど
        Vector3 itemPos = item->GetWorldPosition();

        if (raySprite.IntersectsAABB(box, itemPos)) {

            obj->SetColor({ 1.0f,0.0f,0.0f,1.0f });

            if (InputBind::IsClick()) {
                return item;
            }


        }
    }
    return nullptr;
}

void ItemManager::UseItemFromSlot(const Vector3& pos)
{

    auto item = GetItem("GoldHeart");

    if (InputBind::IsClick()) {
        if (item && !item->isUsed_ && item->isGet_) {
            SoundManager::PlayCorrectSE();
            item->Use();
            Vector3 offset = { -0.3f,0.5f,0.01f };
            Vector3 endOffset = { -0.3f,0.3f,0.01f };
            item->SetStartEndPos(pos + offset, pos + endOffset);
            item->SetRotate({ 4.7f,1.57f,0.0f });

        }
    }

}
