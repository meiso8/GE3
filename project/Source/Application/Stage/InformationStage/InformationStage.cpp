#include "InformationStage.h"

#include"InputBind.h"

InformationStage::InformationStage()
{
    room_ = std::make_unique<InformationRoom>();
    //ゲート
    gate_ = std::make_unique<Gate>();
    //ゲート用センサー
    gateSensor_ = std::make_unique<GateSensor>();

    gateSensor_->SetParent(gate_->GetWorldTransform());
    //チケット番号システム
    passwordText_ = std::make_unique<PasswordText>();
    
}

void InformationStage::Initialize()
{

    StageTransitionInitialize();

    if (isInitialize_) {
        return;
    }
    room_->Init();
    room_->Update();

    passwordText_->Initialize();

    isGenerateTicket_ = false;

    isInitialize_ = true;

}

void InformationStage::StageTransitionInitialize()
{

    //ステージのロード
    LoadAndCreateObject("Information_objectEditor");

    memoManager_->GenerateMemos({ });
    // ミイラ前に移動
    player_->Init({ 3.25f, 0.0f, -3.25f });

    gate_->Initialize();
    gateSensor_->Initialize();
}

void InformationStage::Update()
{
    room_->Update(); 

    gate_->SetHitSensor(gateSensor_->GetHitPlayer());
    //セットした後毎フレーム当たり判定を戻す
    gateSensor_->Update();

    auto item = itemManager_->GetItem("Ticket");

    if (item&& item->IsGet()) {
        //もし、チケットを手に入れたら
        gate_->SetCanOpen(true);
    }

    gate_->Update();

    //パスワードシステム
    passwordText_->Update();

    if (passwordText_->GetIsUnLock()) {
        if (!isGenerateTicket_) {
            isGenerateTicket_ = true;
            itemManager_->GenerateItems({ "Ticket" });
        }
    }
}

void InformationStage::Draw(Camera& camera)
{
    room_->Draw(camera);
    gate_->Draw(camera);
}

void InformationStage::CheckCollision(CollisionManager& collisionManager)
{


    //火のコライダーと当たっていたらがあるところで

    for (auto& obj : objects_) {

        if (obj->collider_->GetCollisionAttribute() == CollisionTag::GetTag("TicketMac")) {
            auto* collider = obj->collider_.get();
            if (player_->GerRaySprite()->Intersect(collider)) {
                if (InputBind::IsClick()) {
                    passwordText_->SetIsActive(true);
                    break;
                }
          
                //Vector3 pos = obj->obj_->GetWorldTransform().GetWorldPosition();
                //Vector3 eyePos = player_->GetEyeWorldTransform().GetWorldPosition();
                //Vector3 offset = { 0.0f,0.75f,0.0f };

                ////ガラスを使う
                //itemManager_->UseItemFromSlot(eyePos, pos + offset, "Glass"
                //);
              /*  break;*/
            }
        }
    }


    //かべ
    for (auto& [type, object] : room_.get()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    collisionManager.AddCollider(gate_.get());
    collisionManager.AddCollider(gateSensor_.get());

}

void InformationStage::DrawSprite()
{
    passwordText_->Draw();
}
