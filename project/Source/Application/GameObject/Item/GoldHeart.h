#pragma once
#include "ItemSlot.h"

class GoldHeart : public Item {
public:
     void Init()override;
    GoldHeart() {
        name_ = "GoldHeart";
        description_ = "何かにはめ込むことで力を発揮する。";
        SetModel("hart");
        object_->SetLightMode(Object3d::kLightModeLReflectance);
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }
    void Update()override;
    void Use() override;
};
