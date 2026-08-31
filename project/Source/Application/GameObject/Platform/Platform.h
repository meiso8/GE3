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
    const uint32_t GetSrvIndex() { return object_->GetSrvIndex(TEXTURE_USAGE_DIFFUSE); }
    virtual void Initialize();
    virtual void Update();
    virtual void Draw();
    WorldTransform& GetWorldTransform() { return object_->GetWorldTransform(); };
    void OnCollision(Collider* collider) override;
    std::unique_ptr<Primitive>cubeMesh_ = nullptr;
    void SetCubeAABB(const AABB& aabb) {
        SetAABB(aabb);
        cubeMesh_->Create(PrimitiveGenerator::CreateCube(aabb));
    };
    void SetCubeSize(const AABB& aabb) {
        cubeMesh_->Create(PrimitiveGenerator::CreateCube(aabb));
    
    };
protected:
    std::unique_ptr<Object3d> object_ = nullptr;
};
