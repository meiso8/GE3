#pragma once

#include<memory>
#include"Object3d.h"
#include"AnimationObject3d.h"
#include"AABB.h"
#include"SkinningModel.h"
#include"Model.h"
#include"Collider.h"
#include"DummyMedjed.h"

class Medjed :public DummyMedjed
{
private:
    std::unique_ptr<AnimationObject3d> aniObj_ = nullptr;
    bool isFind_ = false;
    bool isHit_ = false;
    Vector3 velocity_ = { 0.0f};

private:

public:
    Medjed();
    void Look(const Vector3& target)override;
    void Init()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void OnCollision(Collider* collider)override;

    WorldTransform& GetWorldTransform()const { return aniObj_->worldTransform_; };
    Vector3 GetWorldPosition() const;
    void GoToTarget(const Vector3& target)override;

    void MoveStart();
    const bool& GetIsFind()const { return isFind_; };
    void SetIsFind(const bool& f) { isFind_ = f; };
    const  bool& GetIsHit() const { return isHit_; };
    void SetColor(const Vector4& color) { aniObj_->SetColor(color); };
};

