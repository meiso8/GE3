#include "EyeCollider.h"
#include"CollisionConfig.h"
#include"TimeManager.h"
#include"Easing.h"
#include<algorithm>
#include"MakeMatrix.h"


void EyePosition::Update()
{
    transform_.matWorld_ = MakeAffineMatrix(transform_.eTransform_.scale, transform_.eTransform_.rotate, transform_.eTransform_.translate)* *parent_;
}


void EyePosition::Initialize()
{
    transform_.Initialize();
    transform_.eTransform_.translate.z = 0.0f;
    transform_.eTransform_.translate.y = 0.0f;

}

Vector3& EyePosition::GetForward()
{
    //前方を取得する
    static Vector3 forward;
    forward = Math::GetForward(GetWorldMatrix());
    return forward;
}


void EyePosition::MouseLook(const float& rotateX)
{
    transform_.eTransform_.rotate.x = Lerp(transform_.eTransform_.rotate.x, rotateX, 0.5f);
}

