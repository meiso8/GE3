#include "Sound.h"
#include"SoundManager.h"

std::vector<uint32_t>Sound::soundHandle_;
Sound* Sound::instance_ = nullptr;

Sound* Sound::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new Sound();
    }
    return instance_;
}

void Sound::Load()
{
    SoundManager* soundManager = SoundManager::GetInstance();
    soundManager->Initialize();
    soundHandle_.resize(SOUNDS);

    //サウンドの読み込み
    soundHandle_[BGM1] = soundManager->Load(L"resources/Sounds/dreamcore.mp3");
    soundHandle_[BGM2] = soundManager->Load(L"resources/Sounds/kiritan.mp3");
    soundHandle_[SE1] = soundManager->Load(L"resources/Sounds/poppo.mp3");
    soundHandle_[SE2] = soundManager->Load(L"resources/Sounds/broken.mp3");
}
