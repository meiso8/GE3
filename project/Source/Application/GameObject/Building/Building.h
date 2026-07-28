#pragma once

#include<memory>
#include"Object3d.h"
#include<unordered_map>

#include"Collider.h"
class Camera;
class Model;

class FieldCollider :public Collider {
public:
    FieldCollider();
    void Update();
    void Draw(Camera& camera);
    void Initialize();
    void OnCollision(Collider* collider)override;
    void SettingAABB(const AABB& aabb);
    void SetPos(const Vector3& pos);
    void SetTexture(const TextureFactory::Handle& handle) { object_->SetTextureHandle(handle); }
    void SetTemperature(const float temp) { object_->SetTemperature(temp); }
private:
    std::unique_ptr <Object3d> object_ = nullptr;
    std::unique_ptr < Primitive > cube_ = nullptr;
};

class Building
{
public:
    enum AABBType {
        Wall0,
        Wall1,
        Wall2,
        Wall3,
        Floor,
    };
protected:
    std::unique_ptr <Object3d> buildingPos_ = nullptr;
    Model* model_ = nullptr;
    std::unordered_map<AABBType, AABB>aabbs_;
    std::unordered_map<AABBType, std::unique_ptr <FieldCollider>>fieldPoses_;
public:
    Building();
    ~Building();
    virtual void Init();
    virtual void SetWallAABB();
    virtual void SetWallPos();
    virtual void Update();
    virtual void Draw(Camera& camera);
    std::unordered_map<AABBType, std::unique_ptr <FieldCollider>>& GetFieldPoses() { return fieldPoses_; };
private:
    //壁の厚みを設定する定数
    const float kWallWidth_ = 30.0f;
    const float kWallMaxHeight_ = 5.0f;
    const float kWallMinHeight_ = -1.0f;
    const float kWallThickness_ = 0.5f;
    const float kFloorThickness_ = 0.375f;
    //壁の座標を設定する
    const float kWallPosXZ_ = 26.0f;
    const float kWallOriginPos_ = 0.0f;
    const float kFloorPosY_ = -kFloorThickness_;
};

