#pragma once
class CollisionManager;
class RaySprite;
#include"SoundFactory.h"
#include<string>

class SoundManager {
public:
    static void InitMedjedScene();
    static void PlayCorrectSE();
    static void PlayCancelSE();
    static void ApperMedjedUpdate();
    static void NotFindMedjedUpdate();
    static void PlayGOGOGOSE();
    static void SetRaySprite(RaySprite* raySprite);
    static void SetCollisionManager(CollisionManager* collisionManager);
    static void Update();
private:
    static void PlayDistanceSE(const std::string tagName, const SoundFactory::TAG soundTag, const float firstSoundDistance, const float maxVol);
    static RaySprite* raySprite_;
    static CollisionManager* collisionManager_;
};
