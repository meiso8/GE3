#include"Camera.h"
#include"MakeMatrix.h"
#include"DirectXCommon.h"
#include"DebugUI.h"

const float Camera::kFovAngle = Math::kQPi;

void Camera::Initialize(const PROJECTION_TYPE& type) {

    cameraData_.projectionType = type;
    cameraData_.nearZ = 0.1f;
    cameraData_.farZ = 1000.0f;
    cameraData_.offset = { 0.0f };

    InitializeTransform();
    UpdateProjectionMatrix();

   cameraData_.sphericalCoordinate.radius = 0.0f;
   cameraData_.sphericalCoordinate.azimuthal = 0.0f;
   cameraData_.sphericalCoordinate.polar = 0.0f;

}

Vector3 Camera::GetWorldPos() {
    return Math::GetWorldTransformByMatrix(cameraMatrix_.worldMat);;
}

void Camera::InitializeTransform()
{
    cameraData_.eTransform.scale = { 1.0f,1.0f,1.0f };
    cameraData_.eTransform.rotate = { 0.0f,0.0f,0.0f };
    cameraData_.eTransform.translate = { 0.0f,0.0f,-10.0f };
    cameraMatrix_.worldMat = MakeIdentity4x4();
}

const Matrix4x4& Camera::GetViewProjectionMatrix() {
    return cameraMatrix_.viewProjectionMat;
}

const Matrix4x4& Camera::GetProjectionMatrixForOutline()
{
    return cameraMatrix_.projectionMat;
}

void Camera::UpdateMatrix() {

#ifdef USE_IMGUI
    DebugUI::CheckCamera(cameraMatrix_, cameraData_,"Main Camera");
#endif

    UpdateWorldMatrix();
    UpdateViewProjectionMatrix();
}

void Camera::UpdateWorldMatrix()
{
    cameraMatrix_.worldMat = MakeAffineMatrix(cameraData_.eTransform.scale, cameraData_.eTransform.rotate, cameraData_.eTransform.translate);
    //カメラデータを挿入
    UpdateData();
}

void Camera::CreateResource()
{
    cameraResource_.CreateBufferResource(L"Camera:cameraResource");

    UpdateData();
}

void Camera::UpdateData()
{
    //書き込むためのアドレスを取得
    cameraResource_.Map();
    cameraResource_.data->worldPosition = GetWorldPos();

}

void Camera::SetScreenSize(const float& width, const float& height)
{
   cameraData_.width = width;
   cameraData_.height = height;
}

Camera::Camera()
{
    SetScreenSize(static_cast<float>(Window::GetClientWidth()), static_cast<float>(Window::GetClientHeight()));
    Initialize(PERSPECTIVE);
    //座標が確定後リソースを作成
    CreateResource();
}

Camera::~Camera()
{
    cameraResource_.Reset();
}


void Camera::UpdateViewMatrix()
{
    cameraMatrix_.viewMat = Inverse(cameraMatrix_.worldMat);
}

void Camera::UpdateProjectionMatrix()
{

    if (cameraData_.projectionType == PERSPECTIVE) {
        //投資投影
        cameraMatrix_.projectionMat = MakePerspectiveFovMatrix(cameraData_.fovAngleY, cameraData_.width / cameraData_.height, cameraData_.nearZ, cameraData_.farZ);

    } else if (cameraData_.projectionType == PARALLEL) {
        //平行投影
        float halfWidth = cameraData_.width * 0.5f;
        float halfHeight = cameraData_.height * 0.5f;

        cameraMatrix_. projectionMat = MakeOrthographicMatrix(halfWidth, halfHeight, -halfWidth, -halfHeight, cameraData_. nearZ, cameraData_.farZ);
    }

    cameraMatrix_.projectionMat.m[3][0] += cameraData_.offset.x;
    cameraMatrix_.projectionMat.m[3][1] -= cameraData_.offset.y;

}

void Camera::UpdateViewProjectionMatrix()
{
    UpdateViewMatrix();

    UpdateProjectionMatrix();

    cameraMatrix_.viewProjectionMat = Multiply(cameraMatrix_.viewMat, cameraMatrix_. projectionMat);
}

