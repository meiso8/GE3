#include "TransformAni.h"
#include"TimeManager.h"
#include<cmath>

void TransformAni::RotateY(WorldTransform& worldTransform, const float& rotateSpeed)
{
    worldTransform.eTransform_.rotate.x = 1.0f;
    worldTransform.eTransform_.rotate.y += TimeManager::DeltaTime() * std::numbers::pi_v<float>*rotateSpeed;
}

void TransformAni::PoyoPoyo(WorldTransform& worldTransform,const float& timer,const float&defaultScale)
{
    float theta = std::numbers::pi_v<float>*10.0f * timer;
    worldTransform.eTransform_.scale.x = defaultScale + cos(theta) * 0.5f;
   worldTransform.eTransform_.scale.y = defaultScale + sin(theta) * 0.5f;

}

void TransformAni::LookTarget(WorldTransform& worldTransform, const Vector3& target)
{
    Vector3 direction = target - worldTransform.GetWorldPosition();
    // 正規化して方向ベクトルにする
    direction = Normalize(direction);
    // Y軸回転（左右）

    worldTransform.eTransform_.rotate.y = std::atan2(direction.x, direction.z);
    // X軸回転（上下）
   worldTransform.eTransform_.rotate.x = -std::asin(direction.y); // 上を向くとマイナスになるように
}
