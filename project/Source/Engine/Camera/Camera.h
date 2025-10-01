#pragma once
#include"Matrix4x4.h"
#include"Transform.h"
#include"Vector2.h"

class Camera {
private:
    Vector3 scale_ = { 1.0f,1.0f,1.0f };
    //XYZ軸回りのローカル回転角
    Vector3 rotation_ = { 0.0f,0.0f,0.0f };
    //ローカル座標
    Vector3 translate_ = { 0.0f,0.0f,0.0f };
    //ビュー行列
    Matrix4x4 viewMatrix_ = {};
    //射影行列
    Matrix4x4 projectionMatrix_ = {};
    float farZ_ = 100.0f;
    float width_{};
    float height_{};
    bool isOrthographic_ = false;
    Vector2 offset_ = { 0.0f };
public:
    /// @brief 初期化
    void Initialize(const float& width, const float& height, const bool& isOrthographic);
    /// @brief 更新
    void Update();

    void SetViewMatrix(const Matrix4x4& matrix) { viewMatrix_ = matrix; };
    void SetProjectionMatrix(const Matrix4x4& matrix) { projectionMatrix_ = matrix; };
    Matrix4x4 GetViewMatrix() { return viewMatrix_; };
    Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; };
    Matrix4x4 GetViewProjectionMatrix();

    void SetTransform(const Transform& transform) {
        scale_ = transform.scale;
        rotation_ = transform.rotate;
        translate_ = transform.translate;
    };
    Vector3& GetTranslate() { return translate_; };
    Vector3& GetRotate() { return rotation_; };
    Vector3& GetScale() { return scale_; };
    Vector2& GetOffset() { return offset_; };

    void SetRotate(const Vector3& rotate) { rotation_ = rotate; };
    void SetRotateY(const float& rotateY) { rotation_.y = rotateY; };
    void SetRotateZ(const float& rotateZ) { rotation_.z = rotateZ; };
    void SetOffset(const Vector2& offset) { offset_ = offset; };
    void SetOrthographic(bool isOrthographic) { isOrthographic_ = isOrthographic; }

    void SetFarZ(const float& farZ) { farZ_ = farZ; };
    void SetTranslate(const Vector3& translate) { translate_ = translate; };
    void SetTranslateXY(const Vector2& translate) { translate_.x = translate.x; translate_.y = translate.y; };

    void SetTranslateX(const float& translateX) { translate_.x = translateX; };
    void SetTranslateY(const float& translateY) { translate_.y = translateY; };
    void SetTranslateZ(const float& translateZ) { translate_.z = translateZ; };
};