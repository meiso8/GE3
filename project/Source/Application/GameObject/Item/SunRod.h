#pragma once
#include"ItemSlot.h"

class SunRod : public Item {
public:
    SunRod() {
        name_ = "SunRod";
        description_ = "太陽の力を宿した神聖な杖。";
        SetModel("sunMedal");
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }

    void Use() override {
        //ActivateSunSeal();
    }
};
