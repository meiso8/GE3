#include "GateSensor.h"

GateSensor::GateSensor()
{
    SetWorldMatrix(transform_);
    // AABBを設定（仮のサイズ）
    SetAABB({ {-0.75f, 0.0f, -1.5f}, {0.75f, 2.0f, 1.5f} });


    SetCollisionAttribute(CollisionTag::GetTag("Sensor"));

    SetCollisionMask(CollisionTag::GetTag("Player"));
}

void GateSensor::Initialize()
{
    transform_.Initialize();
}

void GateSensor::Update()
{
    isHitPlayer_ = false;
    WorldTransformUpdate(transform_);

}

void GateSensor::OnCollision(Collider* collider)
{
    isHitPlayer_ = true;
}
