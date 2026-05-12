#include "Building.h"
#include "Model.h"
#include "Camera.h"
#include "Input.h"
#include "Texture.h"
#include"CollisionConfig.h"
#include"AABB.h"
#include"TimeManager.h"
Building::Building() {

    model_ = ModelManager::GetModel("mummyRoom");

    buildingPos_ = std::make_unique<Object3d>();
    buildingPos_->Create();
    buildingPos_->SetMesh(model_);
    buildingPos_->SetLightMode(kLightModeNone);


    fieldPoses_[Wall0] = std::make_unique<FieldCollider>();
    fieldPoses_[Wall1] = std::make_unique<FieldCollider>();
    fieldPoses_[Wall2] = std::make_unique<FieldCollider>();
    fieldPoses_[Wall3] = std::make_unique<FieldCollider>();
    fieldPoses_[Floor] = std::make_unique<FieldCollider>();
    fieldPoses_[Floor]->SetCollisionAttribute(kCollisionFloor);

    for (const auto& [type, object] : fieldPoses_) {
        if (type == Floor) {
            object->SetTexture(TextureFactory::WHITE_1X1);
        } else {
            object->SetTexture(TextureFactory::TEST3);
        }
    }


}

void Building::Init()
{
    buildingPos_->Initialize();
    model_->ResetTextureHandle();

    for (const auto& [type, pos] : fieldPoses_) {
        pos->Initialize();
    }

    SetWallAABB();

    SetWallPos();
}

void Building::SetWallAABB()
{
    // 奥の壁
    aabbs_[Wall0] = {
        {-kWallWidth_,kWallMinHeight_, -kWallThickness_},
        { kWallWidth_, kWallMaxHeight_, kWallThickness_}
    };

    // 手前の壁
    aabbs_[Wall1] = aabbs_[Wall0];

    // 左の壁
    aabbs_[Wall2] = {
        {-kWallThickness_,kWallMinHeight_,-kWallWidth_},
        { kWallThickness_,kWallMaxHeight_, kWallWidth_}
    };

    // 右の壁
    aabbs_[Wall3] = aabbs_[Wall2];

    // 床
    aabbs_[Floor] = {
        {-kWallWidth_, kWallMinHeight_, -kWallWidth_},
        { kWallWidth_, kFloorThickness_,  kWallWidth_}
    };

    for (const auto& [type, pos] : fieldPoses_) {
        pos->SettingAABB(aabbs_[type]);
    }
}

void Building::SetWallPos()
{
    // 前後左右の壁と床の位置を設定
    fieldPoses_[Wall0]->SetPos({ kWallOriginPos_,kWallOriginPos_, -kWallPosXZ_ }); // 奥の壁
    fieldPoses_[Wall1]->SetPos({ kWallOriginPos_, kWallOriginPos_, kWallPosXZ_ }); // 手前の壁
    fieldPoses_[Wall2]->SetPos({ -kWallPosXZ_,kWallOriginPos_, kWallOriginPos_ }); // 左の壁
    fieldPoses_[Wall3]->SetPos({ kWallPosXZ_,kWallOriginPos_, kWallOriginPos_ });  // 右の壁
    fieldPoses_[Floor]->SetPos({ kWallOriginPos_, kFloorPosY_, kWallOriginPos_ });  // 床

}

void Building::Update()
{
    for (const auto& [type, pos] : fieldPoses_) {
        pos->Update();
    }
}

void Building::Draw(Camera& camera)
{
    for (const auto& [type, pos] : fieldPoses_) {
        pos->Draw(camera);
    }
}

FieldCollider::FieldCollider()
{
    cube_ = std::make_unique<Primitive>();
    cube_->Create(PrimitiveGenerator::CreateCube());

    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMesh(cube_.get());

    SetCollisionAttribute(kCollisionWall);
    SetCollisionMask(kCollisionPlayer | kCollisionPlayerEye | kCollisionEnemy | kCollisionMedjed | kCollisionMummy);
    SetWorldMatrix(object_->worldTransform_.matWorld_);
}

void FieldCollider::Update()
{
    object_->GetUVTransform().translate.x -= std::numbers::pi_v<float> *0.0625f * 0.125f * kInverseFPS;
    object_->UpdateUV();

    object_->Update();
    ColliderUpdate();
}

void FieldCollider::Draw(Camera& camera)
{
    object_->Draw(camera);

    ColliderDraw(camera);
}

void FieldCollider::Initialize()
{
    object_->Initialize();
    object_->GetUVScale().x = 4.0f;
}

void FieldCollider::OnCollision(Collider* collider)
{

}

void FieldCollider::SettingAABB(const AABB& aabb)
{
    SetAABB(aabb);
    object_->worldTransform_.scale_ = aabb.max - aabb.min;
}

void FieldCollider::SetPos(const Vector3& pos)
{
    object_->worldTransform_.translate_ = pos;
}
