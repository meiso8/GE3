#include "Medjed.h"
#include"ModelManager.h"
#include"MakeMatrix.h"
#include"SkinningModel.h"
#include"CollisionConfig.h"
#include"Sound.h"
#include"TransformAni/TransformAni.h"
#include"TimeManager.h"
#include<algorithm>
#include"Easing.h"
#include"DebugUI.h"

void Medjed::OnCollision(Collider* collider)
{

    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {

        if (!isHit_) {
            isHit_ = true;
            if (!isFind_) {
                Sound::PlayOriginSE(SoundFactory::VOICE_Asobimasyo, 0.5f);
            }
        }

    }


    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Wall") || collider->GetCollisionAttribute() == CollisionTag::GetTag("DummyMedjed")) {
        velocity_ *= -0.8f;
        ResolveCollision(aniObj_->GetTransform().translate, velocity_, GetCollisionInfo());
    }

    OnCollisionCollider();
}

Vector3 Medjed::GetWorldPosition() const
{
    return aniObj_->GetWorldTransform().GetWorldPosition();
}

void Medjed::GoToTarget(const Vector3& target)
{
    auto& transform = aniObj_->GetTransform();
   transform.translate = Lerp(transform.translate, target, 0.05f);
}


void Medjed::MoveStart()
{
    aniObj_->InitTime();
    //アニメーションをセットするよ
    aniObj_->SetAnimation("Swing");
}

Medjed::Medjed() {

    model_ = ModelManager::GetModel("medjed");
   
    aniObj_ = std::make_unique<AnimationObject3d>();
    aniObj_->Create();
    aniObj_->SetMeshAndMaterial(model_);
    aniObj_->SetTemperature(1.0f);
    aniObj_->SetModelAndLoadAnimation(model_);

    SetAABB(localAABB_);
    SetCollisionAttribute(CollisionTag::GetTag("Medjed"));
    SetCollisionMask(
        CollisionTag::GetTag("Player")
        | CollisionTag::GetTag("Wall") |
        CollisionTag::GetTag("DummyMedjed")
    );
    SetWorldMatrix(aniObj_->GetWorldTransform());
}

void Medjed::Look(const Vector3& target)
{
    Vector3 direction = target - GetWorldPosition();

   
    aniObj_->GetTransform().rotate.y = std::atan2(direction.x, direction.z); // Y軸回転（ラジアン）
}

void Medjed::Update()
{

    if (IsHide()) {
        return;
    }

    isHit_ = false;
    if (isFind_) {
        aniTimer_ += TimeManager::DeltaTime() * 0.25f;
        aniTimer_ = std::clamp(aniTimer_, 0.0f, 1.0f);
        aniObj_->SetColor({ 1.0f,1.0f,1.0f,Easing::EaseInOut(0.0f,1.0f,aniTimer_) });
    } else {
        SetColor({ 1.0f,1.0f,1.0f,0.0f });
    }

    aniObj_->UpdateAniTimer();
    aniObj_->Update();
    ColliderUpdate();

#ifdef USE_IMGUI

    ImGui::Begin("Debug");
    ImGui::Checkbox("isFinedMedjed", &isFind_);
    ImGui::End();

#endif
}
void Medjed::Init()
{

    velocity_ = { 1.0f,0.0f,1.0f };
    hideTimer_ = 0.0f;
    aniTimer_ = 0.0f;
    isHit_ = false;
    isFind_ = false;
    aniObj_->Initialize();
    aniObj_->SetAnimation("Idle");
    aniObj_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
    aniObj_->SetObjectName("Medjed");
    aniObj_->RegisterObject();
}

void Medjed::Draw(Camera& camera)
{
    if (IsHide()) {
        return;
    }
    /*   ColliderDraw(camera);*/
    aniObj_->Draw(camera);


}
