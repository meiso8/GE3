#include "Ticket.h"
#include"Object3d.h"

void Ticket::Init()
{
    aniTimer_ = 0.0f;
    isAnimEnd_ = false;
    isGet_ = false;
    isUsed_ = false;

    isMelt_ = false;

    object_->Initialize();
    object_->SetTranslate({ -3.0f,0.1f,-3.0f });
    object_->SetRotate({ 0.0f,0.1f,0.0f });
    object_->SetTemperature(0.5f);

     screenEndSize_ = 0.03125f*0.75f;

    startPos_ = { 0.0f };
    endPos_ = { 0.0f };
}
