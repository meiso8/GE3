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
#include"Sound.h"

class Model;
class Camera;
enum LightMode;


class Enemy :public Collider
{

public:
    enum PHASE {
        APPEAR,
        ROUND,
        FIREBALL,
        ALPHA_WALK,
        BEAM,
        EXIT,
        MAX_PHASE
    };


    struct ColliderGroup {
        Matrix4x4 matrix_;
        std::unique_ptr<Collider> collider_ = nullptr;
    };


public:
    //体の位置
    AnimationObject3d bodyPos_;

public:

    //コンストラクタ
    Enemy();
    //初期化
    void Init();
    //描画
    void Draw(Camera& camera, const LightMode& lightMode);
    //更新
    void Update();

    void SoundFootStep(const SoundFactory::TAG tag);

    //フラグのカプセル化
    void SetIsApper(const bool& flag) { isAppear_ = flag; }
    const bool& GetIsApper() { return isAppear_; }
    bool GetIsShotStart() { return isShotStart_; }
    void SetIsShotStart(const bool flag) { isShotStart_ = flag; }

    Vector3 GetWorldPos() { return  bodyPos_.GetWorldTransform().GetWorldPosition(); }
    WorldTransform& GetWorldTransform() { return bodyPos_.GetWorldTransform(); }
    void OnCollision(Collider* collder)override;

    std::unordered_map <std::string, ColliderGroup>& GetColliderGroup() { return colliders_; };
    std::unordered_map < std::string, Matrix4x4>& GetEyeMats() { return eyeMats_; };
    //ターゲットの設定
    void SetTarget(Vector3& target) { target_ = &target; };
    //ターゲットに向かうベクトル
    Vector3 GetToTarget();
    Vector3* GetTarget() { return target_; };
    HPs* GetHpsPtr() { return &characterState_.hps; }
    const bool& GetIsDead() { return characterState_.isDead; }
    const PHASE GetPhase() { return phase_; }



private:
    //キャラクターの共通でもつ状態
    CharacterState characterState_;
    //フラグ
    bool isShotStart_ = false;
    bool isAppear_ = false;
    //フェーズのタイマー
    float phaseTimer_ = 0.0f;
    float actionTime_ = 0.0f;

    //目標地点
    Vector3* target_ = nullptr;
    //モデル
    Model* model_;

    Circle enemyRoundCircle_ = { {0.0f,0.0f,0.0f} ,10.0f };
    Circle enemyFieldCircle_ = { {0.0f,0.0f,0.0f} ,9.0f };

    //コライダーのマップ
    std::unordered_map <std::string, ColliderGroup>colliders_;

    //メンバ関数ポインタテーブル
    std::unordered_map<PHASE, std::function<void()>> UpdateActions_;
    PHASE phase_ = PHASE::APPEAR;
    Vector3 velocity_ = { 0.0f };

    Vector3 startPos_ = { 0.0f };
    Vector3 endPos_ = { 0.0f };
    Vector3 startScale_ = { 0.0f,0.0f,0.0f };

    float poyoAnimTimer_ = 0.0f;
    float endRotateY_ = 0.0f;
    float startRotateY_ = 0.0f;
    float roundSpeedY = 1.0f;

    std::unordered_map < std::string, Matrix4x4>eyeMats_;

private:
    //フェーズの設定
    void SetPhase(const PHASE phase);
    //各フェーズ
    void Appear();
    void Round();
    void Fireball();
    void Exit();
    void AlphaWalk();
    void Beam();

    void UpdateTimer();
    void Look();
    bool PoyoPoyoUpdateAndGetEnd(const float& endTimer = 0.25f);
    void HitUpdate();
    void LerpScale();
    void RotateY(const float& timer);
};

