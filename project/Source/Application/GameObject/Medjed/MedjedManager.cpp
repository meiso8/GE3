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

void MedjedManager::RayCastHit(RaySprite& raySprite) {

    if (GetIsApperMedjed()) {
        return;//一旦ここでリターンする
    }

    for (auto& medjed : dummyMedjeds_) {

        AABB aabb = GetAABBWorldPos(medjed.get());
        float dist = 5.0f;
        if (auto correctMedjed = dynamic_cast<Medjed*>(medjed.get())) {
            dist = 10.0f;

        }
       
        if (raySprite.IntersectsAABB(aabb, medjed->GetWorldTransform().GetWorldPosition(), dist)) {
            if (auto correctMedjed = dynamic_cast<Medjed*>(medjed.get())) {
                medjed->SetColor({ 1.0f,1.0f,1.0f,0.5f });
            } else {
                medjed->SetColor({ 1.0f,0.0f,0.0f,1.0f });
            }

    
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
                        Sound::PlayOriginSE(SoundFactory::VOICE_Sottizyanaiwa, 0.5f);
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
    enemy_->SetTarget(*targetPos_);
    GetMedjed()->MoveStart();
}

void MedjedManager::Draw(Camera& camera)
{
    for (auto& dummyMedjed : dummyMedjeds_) {


        if (auto medjed = dynamic_cast<Medjed*>(dummyMedjed.get())) {
            continue;
        }

        dummyMedjed->Draw(camera);
    }

    GetMedjed()->Draw(camera);

    enemy_->Draw(camera, kLightModeHalfL);


}

void MedjedManager::UpdateEnemyApperTime()
{
    if (enemyApperTime_ >= kEnemyApperMaxTime_) {
        return;
    }

    enemyApperTime_ += Time::DeltaTime();
    enemyApperTime_ = std::clamp(enemyApperTime_, 0.0f, kEnemyApperMaxTime_);

    if (enemyApperTime_ >= kEnemyApperMaxTime_) {
        GetEnemy()->SetIsApper(true);
    }
}

void MedjedManager::UpdateMedjedIfNotFind()
{
    //メジェド一つだけ
    GetMedjed()->Look(*targetPos_);

}

void MedjedManager::UpdateMedjedIfFind()
{
    for (auto& medjed : dummyMedjeds_) {

        medjed->Look(*targetPos_);

        if (enemyApperTime_ >= 5.0f) {
            medjed->GoToTarget(enemy_->GetWorldPos());
        }
        if (GetIsApperMedjed()) {
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

    for (auto& locker : dummyMedjeds_) {
        locker->Init();

        Vector2 pos;

        while (true) {

            if (auto medjed = dynamic_cast<Medjed*>(locker.get())) {
                Random random;
                random.SetMinMax(rangeMin, rangeMax);
                pos.x = random.Get();
                random.SetMinMax(rangeMin, rangeMin*0.5f);
                pos.y = random.Get(); // Z座標として使う 
            } else {
                Random random;
                random.SetMinMax(rangeMin, rangeMax);
                pos.x =   random.Get(); 
                pos.y =   random.Get(); // Z座標として使う 
            }
            
            if (!IsOverlapping(pos, placedPositions)) {
                placedPositions.push_back(pos);
                locker->GetWorldTransform().translate_ = { pos.x, 0.0f, pos.y };
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