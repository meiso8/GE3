#pragma once
#include "../Item.h"
class Scarab :
    public Item
{
public:
    Scarab() {
        name_ = "Scarab";
        description_ = "フンコロガシのスカラベ";
        SetModel("scarab.obj");
        object_->SetObjectName(name_);
        object_->RegisterObject();
    }

    ~Scarab() { object_->UnRegisterObject(); }
    void Use() override {

    }
};

