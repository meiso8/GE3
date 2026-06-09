#include "TransformAni.h"
#include"TimeManager.h"
#include<cmath>
void TransformAni::RotateY(WorldTransform& worldTransform, const float& rotateSpeed)
{
    worldTransform.rotate_.x = 1.0f;
    worldTransform.rotate_.y += Time::DeltaTime() * std::numbers::pi_v<float>*rotateSpeed;
}

void TransformAni::PoyoPoyo(WorldTransform& worldTransform,const float& timer,const float&defaultScale)
{
    float theta = std::numbers::pi_v<float>*10.0f * timer;
    worldTransform.scale_.x = defaultScale + cos(theta) * 0.5f;
   worldTransform.scale_.y = defaultScale + sin(theta) * 0.5f;

}

void TransformAni::LookTarget(WorldTransform& worldTransform, const Vector3& target)
{
    Vector3 direction = target - worldTransform.GetWorldPosition();
    // 正規化して方向ベクトルにする
    direction = Normalize(direction);
    // Y軸回転（左右）
    worldTransform.rotate_.y = std::atan2(direction.x, direction.z);
    // X軸回転（上下）
   worldTransform.rotate_.x = -std::asin(direction.y); // 上を向くとマイナスになるように
}
