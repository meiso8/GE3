#pragma once

#include<memory>
#include"Object3d.h"
#include"Collider.h"
#include"AABB.h"

class Model;
class DummyMedjed :public Collider {

private:
    std::unique_ptr < Object3d> object3d_;
    float rotateRange_ = 6.28f;
    float startRotateY_ = 0.0f;
    float startPosY_ = 0.0f;

protected:
    Model* model_ = nullptr;
    static inline const  AABB localAABB_ = { .min = {-0.2f,0.0f,-0.2f},.max = {0.2f,1.5f,0.2f} };
    float aniTimer_ = 0.0f;
    float hideTimer_ = 0.0f;
public:
    DummyMedjed();
    virtual void Init();
    virtual void Draw(Camera& camera);
    virtual void Update();
    void OnCollision(Collider* collider)override;
    virtual void Look(const Vector3& target);
    virtual WorldTransform& GetWorldTransform() const {
        return object3d_->GetWorldTransform();
    };

    virtual void GoToTarget(const Vector3& target);
    virtual void Hide();
    const bool IsHide() { return hideTimer_ >= 1.0f; };
    virtual void SetColor(const Vector4& color) { object3d_->SetColor(color); };
};

