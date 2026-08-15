#include "SoundManager.h"
#include"Sound.h"
#include"TimeManager.h"
#include "Collider.h"
#include"Player/RaySprite.h"
#include "CollisionManager.h"
#include"InputBind.h"
#include"../StageManager/StageManager.h"
#include "ItemManager/ItemManager.h"

RaySprite* SoundManager::raySprite_ = nullptr;
CollisionManager* SoundManager::collisionManager_ = nullptr;
bool SoundManager::isMedjedApper_ = false;
float SoundManager::bgmVolFactor_ = 0.0f;

std::unordered_map<std::string, std::function<void()>> SoundManager::UpdateFunctions_ = {

      {"AmenStage",SoundManager::AmenUpdate},
      {"WaterStage",SoundManager::WaterUpdate},
      {"MedjedStage",SoundManager::MedjedUpdate},
      {"MummyStage",SoundManager::MummyUpdate},
      {"AnubisStage",SoundManager::AnubisUpdate},
      {"MeltStage",SoundManager::MeltUpdate},
     {"BastetStage",SoundManager::BastetUpdate},

};

void SoundManager::SetRaySprite(RaySprite* raySprite) {
    raySprite_ = raySprite;
}

void SoundManager::SetCollisionManager(CollisionManager* collisionManager)
{
    collisionManager_ = collisionManager;
}

void SoundManager::InitSound()
{
    Sound::StopAllSound();
    Sound::SetBGMVolume(0.1f);

    isMedjedApper_ = false;
    bgmVolFactor_ = 0.0f;
}

void SoundManager::PlayCorrectSE()
{
    Sound::PlayOriginSE(SoundFactory::CORRECT);
}

void SoundManager::PlayGOGOGOSE()
{
    Sound::PlayOriginSE(SoundFactory::GOGOGO);
}

void SoundManager::PlayCancelSE()
{
    Sound::PlayOriginSE(SoundFactory::BUZZER);
}


void SoundManager::Update()
{
    //サーモグラフィー中はbgmを一律小さくする

    if (ItemManager::IsGetSolarDisc() && InputBind::IsClickR()) {
        //サーモグラフィ
        Sound::PlayOriginSE(SoundFactory::Thermography, 2.0f);
    }

    if (ItemManager::IsGetSolarDisc() && InputBind::IsClickPressR()) {
        Sound::PlayLoopSE(SoundFactory::LookFor, 0.25f);
        bgmVolFactor_ = 0.0f;

    } else {
        bgmVolFactor_ += TimeManager::DeltaTime();
        bgmVolFactor_ = std::clamp(bgmVolFactor_, 0.0f, 1.0f);

        Sound::Stop(SoundFactory::LookFor);
    }

    //最近のステージ
    std::string currentStage = StageManager::GetInstance()->GetCurrentStageName();
    if (UpdateFunctions_.count(currentStage) > 0) {
        UpdateFunctions_[currentStage]();
    }
    //炎の近くなら鳴る
    PlayDistanceSE("Fire", SoundFactory::Fire, 20.0f, 5.0f);
}

void SoundManager::PlayDistanceSE(
    const std::string tagName,
    const SoundFactory::TAG soundTag,
    const float firstSoundDistance,
    const float maxVol
)
{
    for (const auto& collider : collisionManager_->GetColliders()) {
        if (collider->GetCollisionAttribute() == CollisionTag::GetTag(tagName)) {
            Vector3 targetPos = collider->CalculateWorldPos();
            Vector3 soundPos = raySprite_->GetRay().origin;
            float distance = Distance(targetPos, soundPos);
            if (distance <= firstSoundDistance) {
                float vol = firstSoundDistance - distance;
                vol /= firstSoundDistance;
                vol = std::clamp(vol, 0.0f, 1.0f);
                Sound::PlayLoopSE(soundTag, vol * maxVol);
                break;
            } else {
                Sound::Stop(soundTag);
                break;
            }
        }
    }
}

void SoundManager::AmenUpdate()
{
    Sound::PlayBGM(SoundFactory::BGM_Sun, bgmVolFactor_);
}

void SoundManager::AnubisUpdate()
{
    Sound::PlayBGM(SoundFactory::BGM_Sun, bgmVolFactor_);
}

void SoundManager::BastetUpdate()
{
    //カメライベント時になる
    PlayDistanceSE("CameraUp", SoundFactory::Piano, 50.0f, bgmVolFactor_*0.25f);
}

void SoundManager::WaterUpdate()
{
    Sound::PlayBGM(SoundFactory::BGM_Sea, bgmVolFactor_);
}

void SoundManager::MedjedUpdate()
{

    if (isMedjedApper_) {
        if (Sound::GetBGMVolume() < 0.5f) {
            float vol = Sound::GetBGMVolume() + TimeManager::DeltaTime() * 0.25f;
            Sound::SetBGMVolume(vol);
        }
        Sound::Stop(SoundFactory::HORROR2);
        Sound::PlayBGM(SoundFactory::BGM_ArabRuins, bgmVolFactor_);

    } else {
        Sound::Stop(SoundFactory::BGM_ArabRuins);
        Sound::PlayBGM(SoundFactory::HORROR2, bgmVolFactor_);
    }
}

void SoundManager::MummyUpdate()
{
}

void SoundManager::MeltUpdate()
{
}
