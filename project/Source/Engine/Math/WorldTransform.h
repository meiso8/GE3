#pragma once

#include"Vector3.h"
#include"Matrix4x4.h"
#include"MakeAffineMatrix.h"

class WorldTransform {
public://一旦public
    Vector3 scale_ = { 0.0f };
    Vector3 rotate_ = {0.0f};
    Vector3 translate_ = { 0.0f };
    Matrix4x4 matWorld_ = {0.0f};
    const WorldTransform* parent_ = nullptr;
public:
    void Initialize();
    void SetTranslation(const Vector3& translation) {
        translate_ = translation
            ;
    }
    void SetRotationX(float rotateX) {
        rotate_.x = rotateX
            ;
    }

    void SetRotationY(float rotateY) {
        rotate_.y = rotateY
            ;
    }

    Vector3& GetRotate() {
        return rotate_;
    };

    Vector3& GetTranslate() {
        return translate_;
    };
};


void WorldTransformUpdate(WorldTransform& worldTransform);


void WorldTransformTRUpdate(WorldTransform& worldTransform, const Vector3& translate, const Vector3& rotate);