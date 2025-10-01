#include "DebugCamera.h"
#include"Inverse.h"
#include"MakeAffineMatrix.h"
#include"MakeRotateMatrix.h"
#include"MakeIdentity4x4.h"
#include"MakeTranslateMatrix.h"
#include"MakePerspectiveFovMatrix.h"
#include"MakeOrthographicMatrix.h"
#include"CoordinateTransform.h"
#include"Multiply.h"
#include<numbers>
#include<cmath>

#define FPS 60

void DebugCamera::Initialize(Input* input, const float& width, const float& height) {

    input_ = input;

    width_ = width;
    height_ = height;

    rotateSpeed_ = std::numbers::pi_v<float> / 20.0f / FPS;
    speed_ = 1.0f;

    viewMatrix_ = Inverse(MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, translation_));
    projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, width_ / height_, 0.1f, 100.0f);

    matRot_ = MakeIdentity4x4();
};

void DebugCamera::Update() {

    InputRotate();
    InputTranslate();

    Matrix4x4 matRotDelta = MakeIdentity4x4();
    matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(deltaRotate_.x));
    matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(deltaRotate_.y));
    matRotDelta = Multiply(matRotDelta, MakeRotateZMatrix(deltaRotate_.z));

    deltaRotate_ = { 0.0f,0.0f,0.0f };

    //累積の回転行列を合成
    matRot_ = Multiply(matRot_, matRotDelta);
    viewMatrix_ = Inverse(Multiply(matRot_, MakeTranslateMatrix(translation_)));

    if (isOrthographic_) {
        //平行投影
        projectionMatrix_ = MakeOrthographicMatrix(0.0f, 0.0f, width_, height_, 0.0f, 100.0f);

    } else {
        //投資投影
        projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, width_ / height_, 0.1f, 100.0f);
    }
}

void DebugCamera::InputTranslate() {


    if (input_->IsPushKey(DIK_A)) {
        MoveX(-speed_);
    }

    if (input_->IsPushKey(DIK_D)) {
        MoveX(speed_);
    }

    if (input_->IsPushKey(DIK_W)) {
        MoveY(speed_);
    }

    if (input_->IsPushKey(DIK_S)) {
        MoveY(-speed_);
    }

    if (input_->IsPushKey(DIK_Q)) {
        MoveZ(-speed_);
    }

    if (input_->IsPushKey(DIK_E)) {
        MoveZ(speed_);
    }
};

void DebugCamera::InputRotate() {

    if (input_->IsPushKey(DIK_R)) {

        if (input_->IsTriggerKey(DIK_UP)) {
            rotateSpeed_ *= -1.0f;
        }

        if (input_->IsPushKey(DIK_X)) {
            deltaRotate_.x = rotateSpeed_;
        }

        if (input_->IsPushKey(DIK_Y)) {
            deltaRotate_.y = rotateSpeed_;
        }

        if (input_->IsPushKey(DIK_Z)) {
            deltaRotate_.z = rotateSpeed_;
        }

    }

};

void DebugCamera::MoveZ(const float& speed) {
    //カメラ移動ベクトル
    Vector3 move = { 0.0f,0.0f,speed };
    translation_ += CoordinateTransform(move, matRot_);
}

void DebugCamera::MoveX(const float& speed) {
    translation_ += CoordinateTransform({ speed, 0.0f, 0.0f }, matRot_);
};

void DebugCamera::MoveY(const float& speed) {
    translation_ += CoordinateTransform({ 0.0f, speed, 0.0f }, matRot_);
};

Matrix4x4 DebugCamera::GetViewProjectionMatrix() {
    return Multiply(viewMatrix_, projectionMatrix_);
};
