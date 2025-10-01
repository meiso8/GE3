#include"Camera.h"
#include"Inverse.h"
#include"MakeAffineMatrix.h"
#include"Multiply.h"
#include"MakePerspectiveFovMatrix.h"
#include"MakeOrthographicMatrix.h"

void Camera::Initialize(const float& width, const float& height, const bool& isOrthographic) {

    viewMatrix_ = Inverse(MakeAffineMatrix(scale_, rotation_, translate_));

    width_ = width;
    height_ = height;
    farZ_ = 100.0f;
    isOrthographic_ = isOrthographic;
    offset_ = { 0.0f };

    if (isOrthographic_) {
        //平行投影
        projectionMatrix_ = MakeOrthographicMatrix(0.0f, 0.0f, width_, height_, 0.0f, farZ_);

    } else {
        //投資投影
        projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, width_ / height_, 0.1f, farZ_);
    }
}

void Camera::Update() {

    viewMatrix_ = Inverse(MakeAffineMatrix(scale_, rotation_, translate_));

    if (isOrthographic_) {
        //平行投影
        projectionMatrix_ = MakeOrthographicMatrix(0.0f, 0.0f, width_, height_, 0.0f, farZ_);

    } else {
        //投資投影
        projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, width_ / height_, 0.1f, farZ_);
        projectionMatrix_.m[3][0] += offset_.x;
        projectionMatrix_.m[3][1] -= offset_.y;
    }

}

Matrix4x4 Camera::GetViewProjectionMatrix() {
    return Multiply(viewMatrix_, projectionMatrix_);
}
