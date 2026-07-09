#pragma once
#include"../Item.h"

class SunRod : public Item {
public:
    SunRod() {
        name_ = "SunRod";
        description_ = "太陽の力を宿した神聖な杖。";
        SetModel("sunMedal.obj");
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }
    ~SunRod() { object_->UnRegisterObject(); }
    void Use() override {
        //ActivateSunSeal();
    }
};
