#pragma once
#include"../Stage.h"

#include "PapyrusWall/PapyrusWall.h"
#include"Water/Water.h"
#include"Block/Block.h"
#include<memory>
#include"Memo/MemoManager.h"

class WaterStage :public Stage
{

private:
    std::unique_ptr<Water>water_ = nullptr;
    std::unique_ptr<PapyrusWall>papyrusWall_ = nullptr;
    std::unique_ptr<BlockMap>blockMap_ = nullptr;

    bool itemApper_ = false;
public:
    WaterStage();
    void Initialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void CheckCollision(CollisionManager& collisionManager)override;
};

