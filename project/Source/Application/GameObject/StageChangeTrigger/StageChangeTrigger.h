#pragma once
#include "Collider.h"
#include <unordered_map>

class StageChangeTrigger : public Collider {
public:
    StageChangeTrigger();
    ~StageChangeTrigger() = default;
    void Create(
        const std::string& meshName,
        const std::string& directoryPath,
        const std::string& nextSceneName, 
        const EulerTransform& transform, 
        const Vector3& center,
        const Vector3& size
    );
    void Update();
    void Draw(Camera& camera);
    // コライダーが何かに当たった時に呼ばれる
    void OnCollision(Collider* collider) override;
private:
    std::shared_ptr<Object3d> object_; 

    static std::unordered_map<uint32_t, StageChangeTrigger*> triggerMap_;
};

