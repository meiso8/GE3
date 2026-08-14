#pragma once
#include"../Item.h"

class Glass : public Item {
public:
    void Init()override;
    Glass() {
        name_ = "Glass";
        description_ = "古代エジプト文明のガラスの壺";
        SetModel("glass.obj");
        object_->SetLightMode(Object3d::kLightModeLReflectance);
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }
    ~Glass() { object_->UnRegisterObject(); }
    void Update()override;
    void Use() override;
};
