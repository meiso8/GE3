#include "MedjedManager.h"
#include"Camera.h"
#include"CollisionConfig.h"
#include <algorithm>
#include"Random.h"
#include"Player/RaySprite.h"
#include <cmath>
#include "Vector2.h" // 必要なら自作の2Dベクトルクラスを用意してね
#include"CollisionManager.h"

#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include"InputBind.h"
#include"TimeManager.h"

MedjedManager::MedjedManager()
{
    enemy_ = std::make_unique<Enemy>();

    dummyMedjeds_.clear();
    dummyMedjeds_.resize(46);

    for (int i = 0; i < dummyMedjeds_.size(); ++i) {
        if (i == 0) {
            //0だけ本物のメジェド様を生成！
            dummyMedjeds_[i] = std::make_unique<Medjed>();
        } else {
            dummyMedjeds_[i] = std::make_unique<DummyMedjed>();
        }
    }
}

MedjedManager::~MedjedManager()
{

}

void MedjedManager::RayCastHit() {

    if (GetIsApperMedjed()) {
        return;//一旦ここでリターンする
    }

    for (auto& medjed : dummyMedjeds_) {

        AABB aabb = ColliderWorldPos::GetAABBWorldPos(medjed.get());
        float dist = 5.0f;
        if (auto correctMedjed = dynamic_cast<Medjed*>(medjed.get())) {
            dist = 10.0f;
        }

        if (raySprite_->Intersect(medjed.get(), dist)) {

            //メジェドざまを当ててないとき
            if (!GetIsFindMedjed()) {
                //Mouseをクリックしたら
                if (InputBind::IsClick()) {

                    if (auto correctMedjed = dynamic_cast<Medjed*>(medjed.get())) {
                        SetIsFindMedjed(true);
                        SoundManager::PlayCorrectSE();
                        Sound::PlaySE(SoundFactory::GOGOGO);
                        return;

                    } else {

                        SoundManager::PlayCancelSE();
                        Sound::PlayOriginSE(SoundFactory::VOICE_Sottizyanaiwa, 2.0f);
                        return;
                    }

                }

            }

        }

    }

}

void MedjedManager::Initialize()
{
    enemyApperTime_ = false;
    PlaceLockersRandomly();
    enemy_->Init();
    enemy_->SetTarget(&raySprite_->GetRay().origin);
    GetMedjed()->MoveStart();
}

void MedjedManager::Draw()
{
    for (auto& dummyMedjed : dummyMedjeds_) {

        if (auto medjed = dynamic_cast<Medjed*>(dummyMedjed.get())) {
            continue;
        }

        dummyMedjed->Draw();
    }

    GetMedjed()->Draw();

    enemy_->Draw( Object3d::kLightModeHalfL);

}

void MedjedManager::UpdateEnemyApperTime()
{
    if (enemyApperTime_ >= dummyMedjedHideTime_) {
        //ダミーメジェドが隠れる時間
        return;
    }

    enemyApperTime_ += TimeManager::DeltaTime();
    enemyApperTime_ = std::clamp(enemyApperTime_, 0.0f, dummyMedjedHideTime_);

    if (enemyApperTime_ >= kEnemyApperMaxTime_) {
        //敵出現時間になったら敵出現させる
        auto* enemy = GetEnemy();
        if (!enemy->GetIsApper()) {
            enemy->SetIsApper(true);
        }
      
    }
}

void MedjedManager::UpdateMedjedIfNotFind()
{
    //メジェド一つだけ
    GetMedjed()->Look(raySprite_->GetRay().origin);

}

void MedjedManager::UpdateMedjedIfFind()
{
    for (auto& medjed : dummyMedjeds_) {

        medjed->Look(raySprite_->GetRay().origin);

        if (enemyApperTime_ >= kEnemyApperMaxTime_) {
            medjed->GoToTarget(enemy_->GetWorldPos());
        }
        if (enemyApperTime_ >= dummyMedjedHideTime_) {
            medjed->Hide();
        }

    }
}

void MedjedManager::Update()
{

    if (GetIsFindMedjed()) {

        UpdateEnemyApperTime();
        UpdateMedjedIfFind();
        enemy_->Update();

    } else {
        UpdateMedjedIfNotFind();
    }

    for (auto& locker : dummyMedjeds_) {
        locker->Update();
    }

}

void MedjedManager::PlaceLockersRandomly() {
    std::vector<Vector2> placedPositions;

    for (auto& dummy : dummyMedjeds_) {
        dummy->Init();

        Vector2 pos;

        while (true) {

            if (auto medjed = dynamic_cast<Medjed*>(dummy.get())) {
                Random random;
                random.SetMinMax(rangeMin, rangeMax);
                pos.x = random.Get();
                random.SetMinMax(rangeMin, rangeMin * 0.5f);
                pos.y = random.Get(); // Z座標として使う 
            } else {
                Random random;
                random.SetMinMax(rangeMin, rangeMax);
                pos.x = random.Get();
                pos.y = random.Get(); // Z座標として使う 
            }

            if (!IsOverlapping(pos, placedPositions)) {
                placedPositions.push_back(pos);
                dummy->GetWorldTransform().eTransform_.translate = { pos.x, 0.0f, pos.y };
                break; // 配置成功！ 
            }
        }
    }
}


bool MedjedManager::IsOverlapping(const Vector2& pos, const std::vector<Vector2>& placedPositions) {
    for (const auto& p : placedPositions) {
        if (Distance(p, pos) < minDistance * minDistance) {
            return true;
        }
    }
    return false;
}


Medjed* MedjedManager::GetMedjed() {

    for (auto& locker : dummyMedjeds_) {

        if (auto medjed = dynamic_cast<Medjed*>(locker.get())) {
            return medjed;
        }
    }
    return nullptr; // 見つからなかった場合
}