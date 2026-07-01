#pragma once

#include<list>
#include<map>

#include"Sprite.h"
#include"Item.h"

class ItemSlot {
public:
    /// @brief 最大スロット数の定数
    static const int kMaxSlots_ = 8;
public:
    ItemSlot();

    void Init();
    void OnTriggerItemPickup(const std::shared_ptr<Item>& item);
    void Update();
    void ToScreen();
    bool AddItem(const std::shared_ptr<Item>& item);
    void UseItem(int index);
    void CombineItems(int indexA, int indexB);
    void DrawUI();
    void Draw();
    void GetAnimation(const std::shared_ptr<Item>& item, const Vector2& screenPos);
    std::array<std::shared_ptr<Item>, kMaxSlots_>& GetItemInSlot() {return  slots_; };
  
private:
    std::unique_ptr<Camera> itemCamera_ = nullptr;
    std::array<std::shared_ptr<Item>, kMaxSlots_> slots_;
    std::array<std::unique_ptr<Sprite>, kMaxSlots_> slotSprites_;

    Matrix4x4 matViewport;
    Matrix4x4 matInverseVPV;

    float width = 0.0f;
    float height = 0.0f;

};
