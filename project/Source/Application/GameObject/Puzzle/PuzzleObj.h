#pragma once

#include"Collider.h"
#include <memory>
#include"Object3d.h"
class PuzzleObj : public Collider {
public:
    PuzzleObj();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
    void SetColor(const Vector4& color) { object_->SetColor(color); };
    Vector3 GetWorldPosition() {
      return  object_->worldTransform_.GetWorldPosition();
    }
private:
    std::unique_ptr<Object3d> object_ = nullptr;
    std::unique_ptr<Primitive> cubeMesh_ = nullptr;
};
