#include "MummyRoom.h"

MummyRoom::MummyRoom()
{
}

void MummyRoom::SetWallPos() {
    constexpr float halfWidth = 11.5f;
    constexpr float halfDepth = 11.5f;
    constexpr float wallThickness = 1.0f;
    constexpr float halfWall = wallThickness * 0.5f;

    fieldPoses_[Wall0]->SetPos({ 0.0f, 0.0f, -halfDepth }); // 奥
    fieldPoses_[Wall1]->SetPos({ 0.0f, 0.0f,  halfDepth }); // 手前
    fieldPoses_[Wall2]->SetPos({ -halfWidth, 0.0f, 0.0f }); // 左
    fieldPoses_[Wall3]->SetPos({ halfWidth, 0.0f, 0.0f }); // 右
    fieldPoses_[Floor]->SetPos({ 0.0f, -halfWall, 0.0f });   // 床（指定通り）
    //fieldPoses_[Ceiling]->SetPos({ 0.0f, (halfHeight + halfWall), 0.0f }); // 天井（必要なら）
}

void MummyRoom::SetWallAABB() {
    AABB wallLongX = {
        { -11.5f, 0.0f, -0.5f },
        {  11.5f, 16.5f, 0.5f }
    };

    AABB wallLongZ = {
        { -0.5f, 0.0f, -11.5f },
        {  0.5f, 16.5f, 11.5f }
    };

    aabbs_[Wall0] = wallLongX;
    aabbs_[Wall1] = wallLongX;
    aabbs_[Wall2] = wallLongZ;
    aabbs_[Wall3] = wallLongZ;

    aabbs_[Floor] = {
        { -11.5f, -0.5f, -11.5f },
        {  11.5f,  0.5f,  11.5f }
    };

    for (const auto& [type, pos] : fieldPoses_) {
        pos->SettingAABB(aabbs_[type]);
    }
}

void MummyRoom::Draw(Camera& camera)
{
    buildingPos_->SetLightMode(kLightModeHalfL);
    buildingPos_->Draw(camera);

     fieldPoses_[Floor]->Draw(camera);
}
