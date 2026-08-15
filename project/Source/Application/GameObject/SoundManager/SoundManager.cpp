#include "SoundManager.h"
#include"Sound.h"
#include"TimeManager.h"
#include "Collider.h"
#include"Player/RaySprite.h"
#include "CollisionManager.h"


RaySprite* SoundManager::raySprite_ = nullptr;
CollisionManager* SoundManager::collisionManager_ = nullptr;

void SoundManager::InitMedjedScene()
{
    Sound::StopAllSound();
    Sound::bgmVolume_ = 0.1f;
}

void SoundManager::PlayCorrectSE()
{
    Sound::PlayOriginSE(SoundFactory::CORRECT);
}


void SoundManager::PlayGOGOGOSE()
{
    Sound::PlayOriginSE(SoundFactory::GOGOGO);
}

void SoundManager::SetRaySprite(RaySprite* raySprite) {
    raySprite_ = raySprite;
}

void SoundManager::SetCollisionManager(CollisionManager* collisionManager)
{
    collisionManager_ = collisionManager;
}

void SoundManager::Update()
{
    PlayDistanceSE("Fire",SoundFactory::Fire,20.0f,5.0f);
}

void SoundManager::PlayDistanceSE(const std::string tagName, const SoundFactory::TAG soundTag, const float firstSoundDistance,const float maxVol)
{
    for (const auto& collider : collisionManager_->GetColliders()) {
        if (collider->GetCollisionAttribute() == CollisionTag::GetTag(tagName)) {
            Vector3 targetPos = collider->CalculateWorldPos();
            Vector3 soundPos = raySprite_->GetRay().origin;
            float distance = Distance(targetPos, soundPos);
            if (distance <= firstSoundDistance) {
                float vol = firstSoundDistance - distance;
                vol /= firstSoundDistance;
                vol=  std::clamp(vol, 0.0f, 1.0f);
                Sound::PlayLoopSE(soundTag, vol * maxVol);
                break;
            } else {
                Sound::Stop(soundTag);
                break;
            }
        }
    }
}


void SoundManager::PlayCancelSE()
{
    Sound::PlaySE(SoundFactory::BUZZER);
}

void SoundManager::ApperMedjedUpdate()
{
    if (Sound::bgmVolume_ < 0.5f) {
        Sound::bgmVolume_ += TimeManager::DeltaTime() * 0.25f;
    }
    Sound::Stop(SoundFactory::HORROR2);
    Sound::PlayBGM(SoundFactory::BGM_ArabRuins);
}

void SoundManager::NotFindMedjedUpdate()
{
    Sound::Stop(SoundFactory::BGM_ArabRuins);
    Sound::PlayBGM(SoundFactory::HORROR2);
}
