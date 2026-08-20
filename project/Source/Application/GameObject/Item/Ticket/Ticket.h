#pragma once
#include "../Item.h"
class Ticket : public Item
{
public:
    Ticket() {
        name_ = "Ticket";
        description_ = "特別展古代エジプト入場券";
        SetModel("ticket.obj");
        object_->SetObjectName(name_);
        object_->RegisterObject();
        useStage_ = "InformationStage";
    }

    ~Ticket() { object_->UnRegisterObject(); }
     void Init();
    void Use() override {
        //特にすることがないが…
        isGet_ = true;

    }
};

