#include "ObjectManager.h"
#include"DebugUI.h"
#include"RenderTexture/RenderTexture.h"

ObjectManager* ObjectManager::GetInstance() {
    static ObjectManager instance;
    return &instance;
}

void ObjectManager::RegisterObject(Object3d* gameObject) {
    if (!gameObject) return;
    if (gameObject->GetObjectID() != 0) {
        //あいでぃーあり
        return;
    }
    // 新しいIDを割り当てる（Allocate）
    uint32_t allocatedID = nextID_++;
    gameObject->SetObjectID(allocatedID);

    // 管理用コンテナに追加
    objects_.push_back(gameObject);
    idMap_[allocatedID] = gameObject;
}

void ObjectManager::UnregisterObject(Object3d* gameObject) {
    if (!gameObject) return;

    uint32_t id = gameObject->GetObjectID();
    idMap_.erase(id);

    auto it = std::remove(objects_.begin(), objects_.end(), gameObject);
    if (it != objects_.end()) {
        objects_.erase(it, objects_.end());
    }
}

Object3d* ObjectManager::FindObjectByID(uint32_t id) {
    auto it = idMap_.find(id);
    if (it != idMap_.end()) {
        return it->second;
    }
    return nullptr; // 見つからない（背景など）
}

void ObjectManager::UpdateAll() {
    for (auto& obj : objects_) {
        obj->Update();
    }
}

void ObjectManager::DrawAll(Camera& camera) {
    for (auto& obj : objects_) {
        obj->Draw(camera);
    }
}

void ObjectManager::ClickObject(Camera& camera)
{

#ifdef USE_IMGUI
    ImGui::Begin("Object3ds");

    clickedID_ = RenderTexture::GetInstance()->GetClickedObjectID();

    ImGui::Text("ClickedID : %d", clickedID_);

    if (clickedID_ != 0) {
        auto* selectedObj = ObjectManager::GetInstance()->FindObjectByID(clickedID_);
        if (selectedObj) {
            ImGuiClass::UpdateGuizmo(camera, *selectedObj);
        }
    }

    ImGui::End();
#endif
}

void ObjectManager::DebugAll()
{

    for (int i = 0; i < objects_.size();++i) {
        std::string name = "Object:" + std::to_string(objects_[i]->GetObjectID());
        DebugUI::CheckObject3d(*objects_[i], name.c_str());
    }
 
}

void ObjectManager::Clear() {
    objects_.clear();
    idMap_.clear();
    nextID_ = 1; // IDのリセット
    clickedID_ = 0;
}