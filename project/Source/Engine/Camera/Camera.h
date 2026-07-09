#pragma once
#include"Transform.h"
#include"Vector2.h"
#include"SphericalCoordinate.h"
#include"Matrix/CameraMatrix.h"
#include"ResourceManager/ResourceManager.h"

enum PROJECTION_TYPE {
    PERSPECTIVE,
    PARALLEL,
};

struct CameraData {

    PROJECTION_TYPE projectionType = PERSPECTIVE;

    float width = 1280.0f;
    float height = 720.0f;

    // 垂直方向視野角
    float fovAngleY = Math::kQPi;
    /// @brief 遠方
    float farZ = 1000.0f;
    /// @brief 
    float  nearZ = 0.1f;
    Vector2 offset = { 0.0f };

    /// @brief オイラー角のトランスフォーム
    EulerTransform eTransform;
    //球面座標系
    SphericalCoordinate sphericalCoordinate;

};

class Camera {
public:
    static const float kFovAngle;

    struct CameraForGPU {
        Vector3 worldPosition;
    };

protected:
    CameraData cameraData_;
    //カメラ行列
    CameraMatrix cameraMatrix_;
    //カメラのGPU用リソース
    Resource<CameraForGPU> cameraResource_;
protected:
    void CreateResource();
    void UpdateData();
    virtual void UpdateViewMatrix();
    virtual void UpdateProjectionMatrix();
public:
    Camera();
    ~Camera();
    void SetScreenSize(const float& width, const float& height);
    void InitializeTransform();
    /// @brief 初期化
    virtual void Initialize(const PROJECTION_TYPE& type = PROJECTION_TYPE::PERSPECTIVE);
    /// @brief 更新
    virtual void UpdateMatrix();
    virtual void UpdateWorldMatrix();

    virtual void UpdateViewProjectionMatrix();

    const Matrix4x4& GetWorldMatrix() { return cameraMatrix_.worldMat; }
    const Matrix4x4& GetViewMatrix() { return cameraMatrix_.viewMat; }
    const Matrix4x4& GetProjectionMatrix() { return cameraMatrix_.projectionMat; };
    const Matrix4x4& GetProjectionMatrixForOutline();
    const Matrix4x4& GetViewProjectionMatrix();
    const PROJECTION_TYPE& GetProjectionType() { return cameraData_.projectionType; };
    const EulerTransform& GetTransform() { return cameraData_.eTransform; }
    const SphericalCoordinate& GetSphericalCoordinate() { return cameraData_.sphericalCoordinate; };

    void SetTransform(const EulerTransform& transform) { cameraData_.eTransform = transform;};
    void SetFovAngleY(const float fovAngle) { cameraData_.fovAngleY = fovAngle; };
    void SetFarZ(const float farZ) { cameraData_.farZ = farZ; };
    void SetNearZ(const float nearZ) { cameraData_.nearZ = nearZ; };
    void SetOffset(const Vector2& offset) { cameraData_.offset = offset; };
    void SetProjectionType(const PROJECTION_TYPE& type) { cameraData_.projectionType = type; };
    void SetWorldMatrix(const Matrix4x4& mat) { cameraMatrix_.worldMat = mat; };
    Vector3 GetWorldPos();

    ID3D12Resource* GetResource() {
        return cameraResource_.Get();
    }

};