#pragma once
#include"../Item.h"

class SunMedal : public Item {
public:
    SunMedal() {
        name_ = "SunMedal";
        description_ = "神聖な力を秘めたメダル。";
        SetModel("sunMedal.obj");
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }
    void Use() override {
        //TryOpenNearbyDoor();
    }
};
