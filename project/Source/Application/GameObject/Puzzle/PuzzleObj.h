#pragma once

#include"Collider.h"
#include <memory>
#include"AnimationObject3d.h"

class PuzzleObj : public Collider {
public:
    PuzzleObj();
    ~PuzzleObj();
    
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
    void SetColor(const Vector4& color) { aniObj_->SetColor(color); };
   
    Vector3 GetWorldPosition() {
      return  aniObj_->GetWorldTransform().GetWorldPosition();
    }
    void SetIsOpen(const bool flag) { isOpen_ = flag; }

private:
    bool isOpen_ = false;
    std::unique_ptr<AnimationObject3d> aniObj_ = nullptr;

};
