#pragma once
class CollisionManager;
class RaySprite;
#include"SoundFactory.h"
#include<string>
#include<functional>
class SoundManager {
public:
    SoundManager();
    static void SetRaySprite(RaySprite* raySprite);
    static void SetCollisionManager(CollisionManager* collisionManager);

    static void InitSound();
    
    static void PlayCorrectSE();
    static void PlayCancelSE();
    static void PlayGOGOGOSE();

    static void SetMedjedApper(const bool flag) { isMedjedApper_ = flag; }

    static void Update();
private:
    static void PlayDistanceSE(const std::string tagName, const SoundFactory::TAG soundTag, const float firstSoundDistance, const float maxVol);
    static RaySprite* raySprite_;
    static CollisionManager* collisionManager_;

    //メンバ関数ポインタテーブル
    static std::unordered_map<std::string, std::function<void()>> UpdateFunctions_;
    static bool isMedjedApper_;

    static float bgmVolFactor_;

    static void AmenUpdate();
    static void AnubisUpdate();
    static void BastetUpdate();
    static void WaterUpdate();
    static void MedjedUpdate();
    static void MummyUpdate();
    static void MeltUpdate();
   


};
