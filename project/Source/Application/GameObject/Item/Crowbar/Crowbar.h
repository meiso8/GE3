#pragma once

#include"../Item.h"

class Crowbar : public Item {
public:

    Crowbar() {
        name_ = "Crowbar";
        description_ = "固く閉ざされた扉をこじ開けることができる。";
        SetModel("crowbarItem.obj");
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }

    std::shared_ptr<Item> CombineWith(const std::shared_ptr<Item>& other) override;

    void Use() override {
        //TryOpenNearbyDoor();
    }
};
