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
        useStage_ = "MeltStage";
    }

    ~Scarab() { object_->UnRegisterObject(); }
    void Use() override {
        isUsed_ = true;
        isGet_ = false;
    }
};

