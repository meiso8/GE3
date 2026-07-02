#pragma once
#include "Collider.h"
#include <unordered_map>
#include"LevelEditor/LevelEditor.h"

class StageChangeTrigger : public Collider {
public:
    StageChangeTrigger();
    ~StageChangeTrigger() = default;
    void Create(const LevelData::StageChangeTriggerData& data
    );
    void Update();
    void Draw(Camera& camera);
    // コライダーが何かに当たった時に呼ばれる
    void OnCollision(Collider* collider) override;
private:
    std::shared_ptr<Object3d> object_; 
    static std::unordered_map<uint32_t, StageChangeTrigger*> triggerMap_;
};

