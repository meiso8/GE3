#pragma once
#include "Collider.h"
#include <memory>

#include"Texture.h"
class Platform : public Collider {
public:
    Platform();
    void SetTextureHandle(const TextureFactory::Handle& handle) {
        object_->SetTextureHandle(handle);
    }
    void SetColor(const Vector4& color) { object_->SetColor(color); };
    const uint32_t GetSrvIndex() { return cubeMesh_->GetSrvIndex(); }
    virtual void Initialize();
    virtual void Update();
    virtual void Draw(Camera& camera);
    WorldTransform& GetWorldTransform() { return object_->worldTransform_; };
    void OnCollision(Collider* collider) override;
    std::unique_ptr<Primitive>cubeMesh_ = nullptr;
    void SetCubeAABB(const AABB& aabb) {
        SetAABB(aabb);
        object_->worldTransform_.scale_ = aabb.max - aabb.min;
    };
protected:
    std::unique_ptr<Object3d> object_;
};
