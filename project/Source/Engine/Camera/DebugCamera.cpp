#include "DebugCamera.h"
#include"MakeMatrix.h"
#include"CoordinateTransform.h"
#include"Lerp.h"
#include<numbers>
#include<cmath>
#include"Input.h"
#include"TimeManager.h"
void DebugCamera::Initialize(const PROJECTION_TYPE& type)
{
    projectionType_ = type;
    farZ_ = 1000.0f;
    nearZ_ = 0.1f;
    offset_ = { 0.0f };

    rotateSpeed_ = std::numbers::pi_v<float> / 20.0f;
    speed_ = 1.0f;

    scale_ = { 1.0f,1.0f,1.0f };
    rotate_ = { 0.0f,0.0f,0.0f };
    translate_ = { 0.0f,0.0f,-30.0f };
    worldMat_ = MakeIdentity4x4();

    viewMat_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
    projectionMat_ = MakePerspectiveFovMatrix(fovAngleY_, width_ / height_, nearZ_, farZ_);

    matRot_ = MakeIdentity4x4();

    sphericalCoordinate_.radius = -30.0f;
    sphericalCoordinate_.azimuthal = 0.0f;
    sphericalCoordinate_.polar = 0.0f;
}

DebugCamera::DebugCamera()
{
    SetScreenSize(static_cast<float>(Window::GetClientWidth()), static_cast<float>(Window::GetClientHeight()));
    Initialize(PERSPECTIVE);
    CreateResource();
}

void DebugCamera::UpdateMatrix() {

    //InputRotate();
    //InputTranslate();
    MouseInputMove();

    Matrix4x4 matRotDelta = MakeIdentity4x4();
    matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(deltaRotate_.x));
    matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(deltaRotate_.y));
    matRotDelta = Multiply(matRotDelta, MakeRotateZMatrix(deltaRotate_.z));

    deltaRotate_ = { 0.0f,0.0f,0.0f };

    //累積の回転行列を合成
    matRot_ = Multiply(matRot_, matRotDelta);
    worldMat_ = MakeAffineMatrix(scale_, rotate_, translate_);
    viewMat_ = Inverse(Multiply(matRot_, worldMat_));

    UpdateProjectionMatrix();
    viewProjectionMat_ = Multiply(viewMat_, projectionMat_);

    //りそーすデータを更新
    UpdateData();
}

void DebugCamera::UpdateProjectionMatrix()
{
    if (projectionType_ == PERSPECTIVE) {
        //投資投影
        projectionMat_ = MakePerspectiveFovMatrix(fovAngleY_, width_ / height_, nearZ_, farZ_);
    } else if (projectionType_ == PARALLEL) {
        //平行投影
        float halfWidth = width_ * 0.5f;
        float halfHeight = height_ * 0.5f;
        scale_ = { 0.01f,0.01f,0.01f };
        projectionMat_ = MakeOrthographicMatrix(halfWidth, halfHeight, -halfWidth, -halfHeight, nearZ_, farZ_);
    }

    projectionMat_.m[3][0] += offset_.x;
    projectionMat_.m[3][1] -= offset_.y;
}

void DebugCamera::InputTranslate() {

    if (Input::IsPressKey(DIK_A)) {
        MoveX(-speed_);
    }

    if (Input::IsPressKey(DIK_D)) {
        MoveX(speed_);
    }

    if (Input::IsPressKey(DIK_W)) {
        MoveY(speed_);
    }

    if (Input::IsPressKey(DIK_S)) {
        MoveY(-speed_);
    }

    if (Input::IsPressKey(DIK_Q)) {
        MoveZ(-speed_);
    }

    if (Input::IsPressKey(DIK_E)) {
        MoveZ(speed_);
    }

};

void DebugCamera::InputRotate() {

    if (Input::IsPressKey(DIK_R)) {

        if (Input::IsTriggerKey(DIK_UP)) {
            rotateSpeed_ *= -1.0f;
        }

        const float deltaTime = Time::DeltaTime();

        if (Input::IsPressKey(DIK_X)) {
            deltaRotate_.x = rotateSpeed_* deltaTime;
        }

        if (Input::IsPressKey(DIK_Y)) {
            deltaRotate_.y = rotateSpeed_ * deltaTime;
        }

        if (Input::IsPressKey(DIK_Z)) {
            deltaRotate_.z = rotateSpeed_ * deltaTime;
        }
    }

};

void DebugCamera::MoveZ(const float& speed) {
    //カメラ移動ベクトル
    translate_ += CoordinateTransform({ 0.0f,0.0f,speed }, matRot_);
}

void DebugCamera::MoveX(const float& speed) {
    translate_ += CoordinateTransform({ speed, 0.0f, 0.0f }, matRot_);
};

void DebugCamera::MoveY(const float& speed) {
    translate_ += CoordinateTransform({ 0.0f, speed, 0.0f }, matRot_);
};

void DebugCamera::MouseInputMove() {

    if (Input::IsPressMouse(2) && Input::IsPressKey(DIK_LSHIFT)) {
        //視点の移動 offset をずらす
        //後でoffsetをくわえる
        Vector2 deltaOffset = { 0.0f,0.0f };
        deltaOffset += Input::GetMousePos();
        const float deltaTime = Time::DeltaTime();
        offset_ += { deltaOffset.x* deltaTime, deltaOffset.y* deltaTime * 2.0f };
    } else if (Input::IsPressMouse(2)) {
        //視点の回転
        //中ボタン押し込み&&ドラッグ
        Input::isDragging_ = true;
    }

    //マウススクロールする //初期位置-30
    sphericalCoordinate_.radius += Input::GetMouseWheel();

    if (!Input::IsPressMouse(2)) {
        Input::isDragging_ = false;
    }

    if (Input::isDragging_) {
        Vector2 currentPos = Input::GetMousePosFiltered();
        const float deltaTime = Time::DeltaTime();
        sphericalCoordinate_.polar += currentPos.x * deltaTime *0.5f;
        sphericalCoordinate_.azimuthal -= currentPos.y * deltaTime *0.25f;
        rotate_.y = sphericalCoordinate_.polar;
        rotate_.z = sphericalCoordinate_.azimuthal;
    }

    translate_ = TransformCoordinate(sphericalCoordinate_);

}