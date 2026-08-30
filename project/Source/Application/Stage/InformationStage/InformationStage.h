#pragma once
#include"../Stage.h"
#include"InformationRoom/InformationRoom.h"
#include"Gate/Gate.h"
#include"GateSensor/GateSensor.h"
#include"UI/PasswordText/PasswordText.h"

class InformationStage :public Stage
{
private:
    //部屋
    std::unique_ptr<InformationRoom>room_ = nullptr;

    //ゲート
    std::unique_ptr<Gate>gate_ = nullptr;
    //ゲート用センサー
    std::unique_ptr<GateSensor>gateSensor_ = nullptr;
    //チケット番号入力システム
    std::unique_ptr<PasswordText>passwordText_ = nullptr;
    bool isGenerateTicket_ = false;
  
public:
    InformationStage();
    void Initialize()override;
    void StageTransitionInitialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void CheckCollision(CollisionManager& collisionManager)override;
    void DrawSprite()override;
};

