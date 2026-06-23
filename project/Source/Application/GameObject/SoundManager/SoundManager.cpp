#include "SoundManager.h"
#include"Sound.h"
#include"TimeManager.h"

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
