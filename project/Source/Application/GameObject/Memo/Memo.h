#pragma once

#include "Collider.h"
#include <memory>

#include"Texture.h"

class Memo : public Collider {

private:
    std::unique_ptr<Primitive>cubeMesh_ = nullptr;
    std::unique_ptr<Object3d> object_ = nullptr;
public:
    Memo();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    WorldTransform& GetWorldTransform() { return object_->GetWorldTransform(); };
    void OnCollision(Collider* collider) override;
    void SetTexture(const TextureFactory::Handle& handle) {
        object_->SetTextureHandle(handle);
    };
    void SetTranslate(const Vector3& translate) {
        object_->SetTranslate(translate);
    }
    void SetCubeSize(const AABB& aabb) {
        SetAABB(aabb);
        object_->SetScale(aabb.max - aabb.min);
        object_->SetMeshAndMaterial(cubeMesh_.get());
    };
    void SetColor(const Vector4& color) { object_->SetColor(color); }
};

