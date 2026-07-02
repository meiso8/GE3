#include "ItemManager.h"
#include"AABB.h"
#include"Line.h"
#include"Vector3.h"
#include "CollisionManager.h"
#include"InputBind.h"
#include"DebugUI.h"
#include"Player/RaySprite.h"
#include"SoundManager/SoundManager.h"
#include "../Item/Crowbar/Crowbar.h"
#include "../Item/SunMedal/SunMedal.h"
#include "../Item/GoldHeart/GoldHeart.h"
#include"../Item/SunRod/SunRod.h"
#include"../Item/SolarDisc/SolarDisc.h"
#include"Line.h"

bool ItemManager::isGetSolarDisc_ = false;

ItemManager::ItemManager()
{

}

void ItemManager::GenerateItems(const std::vector<std::string>& itemNames)
{
    // 1. 前のステージの「未取得」のアイテムだけを削除（所持品は残す）
    for (auto it = items_.begin(); it != items_.end(); ) {
        if (!it->second->IsGet()) {
            it = items_.erase(it); // 未取得なら削除してイテレータを進める
        } else {
            ++it;
        }
    }

    for (const auto& name : itemNames) {
  
        // すでに存在しているなら生成をスキップ
        if (items_.find(name) != items_.end()) {
            continue;
        }     
        
        std::shared_ptr<Item> item = nullptr;

        if (name == "Crowbar") {
            item = std::make_shared<Crowbar>();
        } else if (name == "SunMedal") {
            item = std::make_shared<SunMedal>();
        } else if (name == "GoldHeart") {
            item = std::make_shared<GoldHeart>();
        } else if (name == "SunRod") {
            item = std::make_shared<SunRod>();
        } else if(name == "SolarDisc") {
            item = std::make_shared<SolarDisc>();
        }

        if (item) {
            item->Init();
            items_[name] = item;
        }
    }
}

void ItemManager::Init() {
    items_.clear(); // 既存のアイテムをクリア
    itemSlot_.Init();
    //太陽円盤取得フラグの初期化
    isGetSolarDisc_ = false;
}

void ItemManager::Update() {
    for (auto& [name, item] : items_) {
        item->Update();
    }

    if (!isGetSolarDisc_) {
        //未取得時のみ探す
        auto item = GetItem("SolarDisc");
        //取得アニメーションの終了を検知したとき
        if (item && item->IsGet()&&item->IsGetAnimEnd()) {
            isGetSolarDisc_ = true;
        };
    }

    itemSlot_.Update();
}

void ItemManager::DrawGetItem()
{
    itemSlot_.Draw();
}

void ItemManager::Draw(Camera& camera) {
  
    for (auto& [name, item] : items_) {
        if (!item) continue;
        //★ 取得済みのアイテムはステージ上には描画しない スロット側で描画する
        if (item->IsGet()) continue;

        item->Draw(camera);
    }

}

void ItemManager::DrawUI() {

    itemSlot_.DrawUI();
}

bool ItemManager::HasItem(const std::string& name) {
    auto item = GetItem(name);
    return item && item->IsGet();
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

        // ★ すでに取得済みのアイテムはレイキャスト（クリック判定）の対象外にする
        if (item->IsGet()) continue;

        const auto& obj = item->GetObject3d();
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

void ItemManager::UseItemFromSlot(const Vector3& pos,const char* name)
{

    auto item = GetItem(name);

    if (InputBind::IsClick()) {

        if (item && !item->IsUsed() && item->IsGet()) {
            SoundManager::PlayCorrectSE();
            item->Use();
            item->SetStartEndPos(pos + item->GetStartPosOffset(), pos + item->GetEndPosOffset());
            item->SetRotate(item->GetUseRotate());
            //スケールの初期化
            item->InitScale();
        }
    }
}
