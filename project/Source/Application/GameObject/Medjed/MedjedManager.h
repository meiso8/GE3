#pragma once
#include"DummyMedjed.h"
#include<memory>
#include <vector>
#include "Medjed.h"
#include"Enemy.h"
class Camera;
class RaySprite;

class MedjedManager
{

private:
    RaySprite* raySprite_ = nullptr;
    std::vector < std::unique_ptr<DummyMedjed>>dummyMedjeds_;
    std::unique_ptr<Enemy>enemy_ = nullptr;

    float lockerWidth = 1.0f;
    float spacing = 0.2f;
    float minDistance = lockerWidth + spacing;
    float rangeMin = -10.0f;
    float rangeMax = 10.0f;

    float enemyApperTime_ = 0.0f;
    const float kEnemyApperMaxTime_  = 4.0f;
    const float dummyMedjedHideTime_ = 6.0f;

private:

    void UpdateEnemyApperTime();
    void UpdateMedjedIfNotFind();
    void UpdateMedjedIfFind();
    void PlaceLockersRandomly();
    bool IsOverlapping(const Vector2& pos, const std::vector<Vector2>& placedPositions);
public:
    void SetRaySprite(RaySprite* raySprite) { raySprite_ = raySprite; };
    MedjedManager();
    ~MedjedManager();
    void RayCastHit();

    void Initialize();
    void Draw();
    void Update();
    const bool& GetIsEnemyDead() {
        return enemy_->GetIsDead();
    }
    std::vector < std::unique_ptr<DummyMedjed>>& GetAllMedjeds() { return dummyMedjeds_; };
    Medjed* GetMedjed();

    const bool& GetIsFindMedjed() {
        return GetMedjed()->GetIsFind();
    };

    const bool& GetIsApperMedjed() {
        return GetEnemy()->GetIsApper();
    };


    void SetIsFindMedjed(const bool& flag) { GetMedjed()->SetIsFind(flag); }

    Enemy* GetEnemy() {
        return enemy_.get();
    }

};

