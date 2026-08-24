#include "InformationStage.h"


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
