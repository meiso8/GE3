#pragma once
#include"Collider.h"
#include <memory>
#include"AnimationObject3d.h"
#include"SkinningModel.h"
#include "../Platform/Platform.h" 

class Mummy : public Collider {
public:
    Mummy();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
    Platform* GetPlatform() { return platform_.get(); }
    void SetIsOpen(const bool& isOpen) { isOpen_ = isOpen; }
    const bool& GetIsOpen() {
        return isOpen_ ;
    };
    const bool& GetIsOpenEnd() {
        return isOpenEnd_;
    };
    virtual WorldTransform& GetWorldTransform()
    {
        return object_->GetWorldTransform();
    }
private:
    std::unique_ptr<Object3d> object_;
    std::unique_ptr<AnimationObject3d> aniObj_;
    Model* coffinModel_ = nullptr;
    std::unique_ptr<Platform> platform_; // 台のコライダーを追加
    bool isOpen_ = false;
    bool isOpenEnd_ = false;
};
