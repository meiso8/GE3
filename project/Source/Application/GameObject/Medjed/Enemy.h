#pragma once
#include"WorldTransform.h"
#include"Object3d.h"
#include"AABB.h"
#include"CharacterState.h"
#include"Circle.h"
#include"Collider.h"
#include<memory>

#include"AnimationObject3d.h"
#include"Model/SkinningModel.h"
class Model;
class Camera;
enum LightMode;


class Enemy :public Collider
{
private:
    //キャラクターの共通でもつ状態
    CharacterState characterState_;

public:

    Enemy();
    void Init();
    void Draw(Camera& camera, const LightMode& lightMode);
    void Update();
    void SetIsApper(const bool& flag) { isAppear_ = flag; }
    const bool& GetIsApper() { return isAppear_; }
    Vector3 GetWorldPos()const
    {
        return bodyPos_.worldTransform_.GetWorldPosition();
    }
    WorldTransform& GetWorldTransform() { return bodyPos_.worldTransform_; }
    void OnCollision(Collider* collder)override;

    void SetTarget(Vector3& target) { target_ = &target; };
    Vector3 GetToTarget() { 
        if (target_ != nullptr) {
            return Normalize(*target_ - bodyPos_.worldTransform_.GetWorldPosition());
        }
        
        return { 0.0f };
    }
    bool isShotStart_ = false;
    //体の位置
    AnimationObject3d bodyPos_;
    HPs* GetHpsPtr() { return &characterState_.hps; }
    const bool& GetIsDead() { return characterState_.isDead; }
private:
   const float kScale_ = 5.0f;
    bool isAppear_ = false;
    float timer_ = 0.0f;
    float actionTime_ = 0.0f;
    float attackTime_ = 10.0f;
    //目標地点
    Vector3* target_ = nullptr;
    //モデル
    Model* model_;

    Circle enemyRoundCircle_ = { {0.0f,0.0f,0.0f} ,10.0f};
    Circle enemyFieldCircle_ = { {0.0f,0.0f,0.0f} ,9.0f };

    enum PHASE {
        APPEAR,
        ROUND,
        FIREBALL,
        ALPHA_WALK,
        EXIT,
        MAX_PHASE
    };

    //メンバ関数ポインタテーブル
    std::unordered_map<PHASE, std::function<void()>> UpdateActions_;
    PHASE phase_ = PHASE::APPEAR;
    Vector3 velocity_ = { 0.0f };

    Vector3 startPos_ = { 0.0f };
    Vector3 endPos_ = { 0.0f };
    float poyoAnimTimer_ = 0.0f;
    float endRotateY_ = 0.0f;
    float startRotateY_ = 0.0f;
    float roundSpeedY = 1.0f;

    const float kApperTime_ = 7.0f;
    const float kApperEndTime_ = 9.0f;
    Vector3 startScale_ = { 0.0f,0.0f,0.0f };

    const float kAlphaWalkTime_ = 5.0f;
    const float kAlphaWalkEndTime_ = 15.0f;

private:
    void SetPhase(PHASE phase);
    void Appear();
    void Round();
    void Fireball();
    void Exit();
    void AlphaWalk();
    void UpdateTimer();
    void Look();
    void PoyoPoyo(const float& endTimer = 0.25f);
    void HitUpdate();
    void LerpScale();
    void RotateY (const float& timer);
};

