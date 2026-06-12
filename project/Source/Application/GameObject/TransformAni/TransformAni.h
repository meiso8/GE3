#pragma once
#include"WorldTransform.h"
#include"Vector3.h"

class TransformAni
{
public:
    static void RotateY(WorldTransform& worldTransform, const float& rotateSpeed);
    static void PoyoPoyo(WorldTransform& worldTransform, const float& timer, const float& defaultScale);
    static void LookTarget(WorldTransform& worldTransform, const Vector3& target);
};