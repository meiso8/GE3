#pragma once
#include"../Item.h"

class SolarDisc : public Item {
public:
    SolarDisc() {
        name_ = "SolarDisc";
        description_ = "見えないものが見えるようになる";
        SetModel("SolarDisc.obj");
        object_->SetLightMode(Object3d::kLightModeLReflectance);
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }

    ~SolarDisc() { object_->UnRegisterObject(); }
   void Init()override;
    void Use() override;
};
