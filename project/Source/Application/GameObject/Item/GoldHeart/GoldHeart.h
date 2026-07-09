#pragma once
#include"../Item.h"

class GoldHeart : public Item {
public:
     void Init()override;
    GoldHeart() {
        name_ = "GoldHeart";
        description_ = "何かにはめ込むことで力を発揮する。";
        SetModel("hart.obj");
        object_->SetLightMode(Object3d::kLightModeLReflectance);
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }
    ~GoldHeart(){ object_->UnRegisterObject(); }
    void Update()override;
    void Use() override;
};
