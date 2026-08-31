#include "Gate.h"
#include"ModelManager.h"
#include"Model.h"
#include"Sound.h"

Gate::Gate()
{
    model_ = ModelManager::LoadModelAndGet("Resource/Models/Gate/gate.gltf");

    aniObj_ = std::make_unique<AnimationObject3d>();

    aniObj_->Create();
    aniObj_->SetMeshAndMaterial(model_);
    aniObj_->SetTemperature(0.2f);
    aniObj_->SetModelAndLoadAnimation(model_);

    SetWorldMatrix(aniObj_->GetWorldTransform().matWorld_);
    //AABBを設定（仮のサイズ）
    SetAABB({ {-0.75f, 0.0f, -0.25f}, {0.75f, 2.0f, 0.25f} });
}

void Gate::Initialize()
{

    canOpen_ = false;
    isHitSensor_ = false;
    isOpen_ = false;
    aniObj_->Initialize();
    aniObj_->SetTranslate({ -1.0f,0.0f,0.0f });
    aniObj_->SetRotate({ 0.0f,Math::kPi,0.0f });
    aniObj_->SetAnimation("Idle");

    SetCollisionAttribute(CollisionTag::GetTag("Wall"));

    SetCollisionMask(
        CollisionTag::GetTag("Player")
        | CollisionTag::GetTag("Enemy"));

}
void Gate::Update()
{
    const auto animationName = aniObj_->GetCurrentAnimation();

    if (canOpen_) {
        if (aniObj_->IsAnimEnd()) {
            //アニメーション終了時

            if (isHitSensor_) {
                if (animationName == "Idle") {
                    Sound::PlayOriginSE(SoundFactory::GATE);
                    //アイドル状態の時開くアニメーション
                    aniObj_->SetAnimation("Open");
                }   else if (animationName == "Open") {
                    //開くアニメーション終了時　オープン
                    isOpen_ = true;
                }

            } else {
                //プレイヤーが任意の場所から離れたら
                if (animationName == "Open") {
                    aniObj_->SetAnimation("Close");
                } else if (animationName == "Close") {
                    //閉じるアニメーション終了時クローズ
                    isOpen_ = false;
                    aniObj_->SetAnimation("Idle");
                }
            }

        }
    }


    if (isOpen_) {
        SetCollisionAttribute(CollisionTag::GetTag("unKnown"));
    } else {
        SetCollisionAttribute(CollisionTag::GetTag("Wall"));
    }

    aniObj_->Update();
    //アニメーション ループしない
    aniObj_->UpdateAniTimer(false);


}

void Gate::Draw()
{
    aniObj_->Draw();
}

void Gate::OnCollision(Collider* collider)
{
}
