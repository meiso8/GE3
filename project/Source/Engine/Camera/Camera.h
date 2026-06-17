#pragma once
#include"Matrix4x4.h"
#include"Transform.h"
#include"Vector2.h"
#include"SphericalCoordinate.h"
#include <string>

#include<wrl.h>
#include<d3d12.h>

struct CameraForGPU {
    Vector3 worldPosition;
};

class Camera {
public:
    const float kFovAngle_ = 45.0f * 3.141592654f / 180.0f;
    Vector3 scale_ = { 1.0f,1.0f,1.0f };
    Vector3 rotate_ = { 0.0f,0.0f,0.0f };
    Vector3 translate_ = { 0.0f,0.0f,0.0f };
    // 垂直方向視野角
    float fovAngleY_ = kFovAngle_;
    float farZ_ = 1000.0f;
   float  nearZ_ = 0.1f;

    Vector2 offset_ = { 0.0f };

    enum PROJECTION_TYPE {
        PERSPECTIVE,
        PARALLEL,
    };

    PROJECTION_TYPE projectionType_ = PERSPECTIVE;
    Matrix4x4 worldMat_;
    //ビュー行列
    Matrix4x4 viewMat_;
    //射影行列
    Matrix4x4 projectionMat_ = { 0.0f };
    Matrix4x4 viewProjectionMat_ = { 0.0f };

    //球面座標系
    SphericalCoordinate sphericalCoordinate_;

protected:

    static float width_;
    static float height_;
    virtual void UpdateViewMatrix();
    virtual void UpdateProjectionMatrix();

    //カメラのGPU用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
    CameraForGPU* cameraData_ = nullptr;

    void CreateResource();
    void UpdateData();

public:
    void SetScreenSize(const float& width, const float& height);
     Camera();
    /// @brief 初期化
    virtual void Initialize(const PROJECTION_TYPE& type = PROJECTION_TYPE::PERSPECTIVE);
    /// @brief 更新
    virtual void UpdateMatrix();
    virtual void UpdateWorldMatrix();

    virtual void UpdateViewProjectionMatrix();
    const Matrix4x4& GetViewMatrix() { return viewMat_; }
    const Matrix4x4& GetProjectionMatrix() { return projectionMat_; };
   Matrix4x4& GetViewProjectionMatrix();
    void SetTransform(const EulerTransform& transform) {
        scale_ = transform.scale;
        rotate_ = transform.rotate;
        translate_ = transform.translate;
    };

    Vector3 GetWorldPos();
    void InitializeTransform();
    const Matrix4x4& GetProjectionMatrixForOutline();
    ID3D12Resource* GetResource() {
        return cameraResource_.Get();
    }

};