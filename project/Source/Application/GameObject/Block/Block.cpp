#include "Block.h"
#include"Easing.h"
#include"TimeManager.h"
#include<algorithm>
#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include"CollisionConfig.h"
#include"InputBind.h"
#include"../System/CollisionManager.h"
#include"../Engine/ImGui/DebugUI.h"
Block::Block()
{
    SetCollisionAttribute(CollisionTag::GetTag("Floor"));
    SetCollisionMask(
          CollisionTag::GetTag("Player")
        | CollisionTag::GetTag("Enemy") 
        | CollisionTag::GetTag("Medjed")
        | CollisionTag::GetTag("DummyMedjed")
    );
    SetAABB({ {-1.0f,-1.0f,-1.0f} ,{1.0f,1.0f,1.0f} });
}

void Block::Initialize()
{
    object_->Initialize();
    aniTimer_ = 0.0f;
    isPush_ = false;
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);

}

void Block::Update()
{
    aniTimer_ += TimeManager::DeltaTime();
    aniTimer_ = std::clamp(aniTimer_, 0.0f, 1.0f);

    if (isPush_) {
        object_->GetTransform().translate.y = Easing::EaseInOut(startPosY_, endPosY_, aniTimer_);
    } else {
        object_->GetTransform().translate.y = Easing::EaseInOut(endPosY_, startPosY_, aniTimer_);
    }

    object_->Update();
    ColliderUpdate();
}

void Block::OnCollision(Collider* collider)
{
    OnCollisionCollider();



}

void Block::SetPos(const Vector3& pos, const float& endOffset)
{
    object_->SetTranslate(pos);
    startPosY_ = pos.y;
    SetEndPos(endOffset);
}

void Block::SetEndPos(const float& endOffset)
{
    endPosY_ = startPosY_ + endOffset;
}

void Block::InitAnitimer()
{
    aniTimer_ = 0.0f;
}

void Block::Reset(const bool isCheckPushBlock)
{
    if (!CanPushBlock()&& isCheckPushBlock) {
        return;
    }

    aniTimer_ = 0.0f;
    isPush_ = false;
}

void Block::RayCastHit(const bool isCheckPushBlock)
{
    if (!InputBind::IsClick()) {
        return;
    }

    if (isPush_) {
        return;
    }

    if (!CanPushBlock()&& isCheckPushBlock) {
        return;
    }

    Sound::PlaySE(SoundFactory::MOVE_ROCK, 1.0f);
    aniTimer_ = 0.0f;
    isPush_ = true;

}
