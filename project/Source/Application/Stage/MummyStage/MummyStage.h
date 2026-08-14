#pragma once
#include"../Stage.h"
#include"Mummy/Mummy.h"
#include<memory>
#include"Player/RaySprite.h"
#include"Papyrus/Papyrus.h"
#include "MummyRoom/MummyRoom.h" 
#include"DummyMummy/DummyMummy.h"
#include<array>
class MummyStage :public Stage
{

public:

private:
    static const int kMaxMummy_ = 13;

    std::unique_ptr<Mummy>mummy_ = nullptr;
    std::unique_ptr<Papyrus> papyrus_ = nullptr;
    std::unique_ptr<MummyRoom> mummyRoom_ = nullptr; 
    std::array<std::unique_ptr<DummyMummy>, kMaxMummy_> dummyMummies_;

public:

    MummyStage();
    ~MummyStage();
    void Initialize()override;
    void StageTransitionInitialize()override;
    void Update()override;
    void Draw(Camera& camera)override;

    bool IsRayCastHit(RaySprite& raysprite);
    void CheckCollision(CollisionManager& collisionManager)override;
    Mummy* GetMummy() {
        return mummy_
            .get();
    };

};

