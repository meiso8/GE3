#pragma once

#include "Collider.h"
#include <memory>
#include"Texture.h"

class Memo : public Collider {

private:
    std::unique_ptr<Primitive>cubeMesh_ = nullptr;
    std::unique_ptr<Object3d> object_ = nullptr;
    const float kOffset_ = 0.25f;
    const float kThickness_ = 0.1f;
public:
    Memo();
    void Initialize();
    void Update();
    void Draw();
    WorldTransform& GetWorldTransform() { return object_->GetWorldTransform(); };
    void OnCollision(Collider* collider) override;
    void SetTexture(const TextureFactory::Handle& handle) {
        object_->SetTextureHandle(handle);
    };
    void SetTranslate(const Vector3& translate) {
        object_->SetTranslate(translate);
    }
    void SetCubeSize(const AABB& aabb) {
        AABB offset = { .min = {-kOffset_,-kThickness_,-kOffset_},.max = {kOffset_,kThickness_,kOffset_} };
        SetAABB(aabb+ offset);
        object_->SetScale(aabb.max - aabb.min);
        object_->SetMeshAndMaterial(cubeMesh_.get());
    };
    void SetColor(const Vector4& color) { object_->SetColor(color); }
};

