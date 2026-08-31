#include "InformationRoom.h"
#include"ModelManager/ModelManager.h"
#include"Model.h"
InformationRoom::InformationRoom() {
    model_ = ModelManager::LoadModelAndGet("Resource/Models/Information/information.obj");
    buildingPos_->SetMeshAndMaterial(model_);
    buildingPos_->SetLightMode(Object3d::LightMode::kLightModeHalfL);
};

void InformationRoom::Draw() {
    buildingPos_->Draw();
}
void InformationRoom::SetWallPos() {

    constexpr float halfWidth = 4.0f;
    constexpr float halfDepth = 4.0f;
    constexpr float wallThickness = 1.0f;
    constexpr float halfWall = wallThickness * 0.5f;

    fieldPoses_[Wall0]->SetPos({ 0.0f, 0.0f, -halfDepth }); // 奥
    fieldPoses_[Wall1]->SetPos({ 0.0f, 0.0f,  halfDepth }); // 手前
    fieldPoses_[Wall2]->SetPos({ -halfWidth, 0.0f, 0.0f }); // 左
    fieldPoses_[Wall3]->SetPos({ halfWidth, 0.0f, 0.0f }); // 右
    fieldPoses_[Floor]->SetPos({ 0.0f, -halfWall, 0.0f });   // 床（指定通り）
};
void InformationRoom::SetWallAABB() {
    AABB wallLongX = {
    { -4.0f, 0.0f, -0.5f },
    {  4.0f, 16.5f, 0.5f }
    };

    AABB wallLongZ = {
        { -0.5f, 0.0f, -4.0f },
        {  0.5f, 16.5f, 4.0f }
    };

    aabbs_[Wall0] = wallLongX;
    aabbs_[Wall1] = wallLongX;
    aabbs_[Wall2] = wallLongZ;
    aabbs_[Wall3] = wallLongZ;

    aabbs_[Floor] = {
        { -4.5f, -0.5f, -4.5f },
        {  4.5f,  0.5f,  4.5f }
    };

    for (const auto& [type, pos] : fieldPoses_) {
        pos->SettingAABB(aabbs_[type]);
    }
};