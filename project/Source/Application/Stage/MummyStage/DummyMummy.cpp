#include "DummyMummy.h"
#include"ModelManager.h"
#include"Model.h"
#include"CollisionConfig.h"
#include"TimeManager.h"
Vector3* DummyMummy::targetPos_ = nullptr;
Model* DummyMummy::model_ = nullptr;

#include"Sound.h"
DummyMummy::DummyMummy()
{
    model_ = ModelManager::GetModel("dummyMummy");

    object_ = std::make_unique<AnimationObject3d>();

    object_->Create();
    object_->SetMeshAndMaterial(model_);
    object_->SetTemperature(1.0f);
    object_->SetModelAndLoadAnimation(model_);
    
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);
    // ミイラのサイズに合わせてAABBを設定（仮のサイズ）
    SetAABB({ {-0.75f, 0.0f, -0.75f}, {0.75f, 2.0f, 0.75f} });
}

void DummyMummy::Initialize()
{
    isHitCollision_ = false;
    isOpen_ = false;
    object_->Initialize();
    Look(*targetPos_);
    object_->SetSkinning(false);

    SetCollisionAttribute(kCollisionWall);
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy | kCollisionMummy);
}

void DummyMummy::Update()
{

    if (isOpen_) {

        float distance = Length(*targetPos_ - GetWorldPos());

        if (distance > 10.0f) {
            //アニメーションも移動もスキップ 
            object_->SetSkinning(false);
            return;
        } else {
            object_->SetSkinning(true);
            if (!isHitCollision_) {
                Sound::PlayOriginSE(SoundFactory::WOO);
            }
        }

        //ループアニメーション
        object_->UpdateAniTimer();
        auto& transform = object_->GetTransform();
        if (isHitCollision_) {
            //倒れる
            transform.rotate.x = Lerp(transform.rotate.x, 1.57f, 0.075f);
            transform.translate.y = Lerp(transform.translate.y, 0.25f, 0.075f);
        } else {
            Look(*targetPos_);
            velocity_ = *targetPos_ - object_->GetWorldTransform().GetWorldPosition();
            velocity_ = Normalize(Vector3{ velocity_.x, 0.0f, velocity_.z });
           transform.translate += velocity_ * Time::DeltaTime();
        }

    }

    object_->Update();

}

void DummyMummy::Draw(Camera& camera)
{
    object_->Draw(camera);

}

void DummyMummy::OnCollision(Collider* collider)
{
    if (isHitCollision_ || !isOpen_) {
        return;
    }

    // 自分自身との衝突は無視 
    if (collider == this) { return; }

    if (collider->GetCollisionAttribute() == kCollisionWall) {
        ResolveCollision(object_->GetTransform().translate, velocity_, GetCollisionInfo());
    }

    if (collider->GetCollisionAttribute() == kCollisionMummy) {
        isHitCollision_ = true;
    }

}

void DummyMummy::SetCollisionType()
{

    SetCollisionAttribute(kCollisionMummy); // ミイラの衝突属性
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy | kCollisionMummy | kCollisionWall); // プレイヤーや壁と衝突
}

Vector3 DummyMummy::GetWorldPos()
{
    return object_->GetWorldTransform().GetWorldPosition();
}

void DummyMummy::Look(const Vector3& target)
{
    Vector3 direction = target - GetWorldPos();
    if (Length(direction) > 0.0f) {
        object_->GetTransform().rotate.y = std::atan2(direction.x, direction.z); // Y軸回転（ラジアン）
    }


}
