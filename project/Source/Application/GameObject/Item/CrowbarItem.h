#pragma once

#include"ItemSlot.h"

class CrowbarItem : public Item {
public:

    CrowbarItem() {
        name_ = "CrowbarItem";
        description_ = "固く閉ざされた扉をこじ開けることができる。";
        SetModel("crowbarItem");
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }

    std::shared_ptr<Item> CombineWith(const std::shared_ptr<Item>& other) override;

    void Use() override {
        //TryOpenNearbyDoor();
    }
};
