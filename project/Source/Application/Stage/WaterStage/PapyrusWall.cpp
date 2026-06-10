#include "PapyrusWall.h"


void PapyrusWall::SetWallPos() {
    constexpr float halfWidth = 11.5f;
    constexpr float halfDepth = 11.5f;
    constexpr float wallThickness = 1.0f;
    constexpr float halfWall = wallThickness * 0.5f;

    fieldPoses_[Wall0]->SetPos({ 0.0f, 0.0f, -(halfDepth ) }); // 奥
    fieldPoses_[Wall1]->SetPos({ 0.0f, 0.0f,  (halfDepth) }); // 手前
    fieldPoses_[Wall2]->SetPos({ -(halfWidth), 0.0f, 0.0f }); // 左
    fieldPoses_[Wall3]->SetPos({ (halfWidth), 0.0f, 0.0f }); // 右
    fieldPoses_[Floor]->SetPos({ 0.0f, -1.5f- halfWall, 0.0f }); // 床
    //fieldPoses_[Ceiling]->SetPos({ 0.0f,  (halfHeight + halfWall), 0.0f }); // 天井
}

PapyrusWall::PapyrusWall()
{
}

void PapyrusWall::Draw(Camera& camera)
{
    buildingPos_->SetLightMode(kLightModeHalfL);
    buildingPos_->Draw(camera);

    /* for (const auto& [type, pos] : fieldPoses_) {
         pos->Draw(camera);
     }*/
}

void PapyrusWall::SetWallAABB() {


    AABB wallLongX = {
        { -11.5f, 0.0f, -0.5f },
        { 11.5f,  16.5f, 0.5f }
    };

    AABB wallLongZ = {
        {-0.5f, 0.0f, -11.5f},
        { 0.5f , 16.5f, 11.5f}
    };


    // 奥の壁（Zマイナス側）
    aabbs_[Wall0] = wallLongX;

    // 手前の壁（Zプラス側）
    aabbs_[Wall1] = wallLongX;

    // 左の壁（Xマイナス側）
    aabbs_[Wall2] = wallLongZ;

    // 右の壁（Xプラス側）
    aabbs_[Wall3] = wallLongZ;

    // 床（Yマイナス側）
    aabbs_[Floor] = {
        {-11.5f, -0.5f, -11.5f},
        { 11.5f, 0.5f,  11.5f}
    };

    // 天井（Yプラス側）←追加
    //aabbs_[Ceiling] = {
    //    {-11.5f, 10.6f, -8.35f},
    //    { 11.5f, 11.6f,  8.35f}
    //};

    for (const auto& [type, pos] : fieldPoses_) {
        pos->SettingAABB(aabbs_[type]);
    }
}
