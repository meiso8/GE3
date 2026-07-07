#pragma once
#include"WorldTransform.h"
#include"Vector3.h"

class TransformAni
{
public:
    static void RotateY(WorldTransform& worldTransform, const float& rotateSpeed);
    static void PoyoPoyo(WorldTransform& worldTransform, const float& timer, const float& defaultScale);
    static void PoyoPoyo(Vector2& size, const float& timer, const float& defaultScale,const float& amplitude);
    static void LookTarget(WorldTransform& worldTransform, const Vector3& target);
};