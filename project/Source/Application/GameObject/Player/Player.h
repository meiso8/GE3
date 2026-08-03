#pragma once

#include"WorldTransform.h"
#include"AnimationObject3d.h"
#include"CharacterState.h"
#include"AABB.h"
#include"Collider.h"
#include"EyeCollider.h"
#include"RaySprite.h"

class Model;
class Camera;
enum LightMode;
class CircleMesh;
class CubeMesh;
class Sprite;


class Player :public Collider
{
public:


    Player();
    ~Player();
    void Init(const Vector3& pos);
    void Draw(Camera& camera);
    void DrawRaySprite();
    void Update();
    void Debug();

    Vector3& GetEyeForward();
    Vector3& GetBodyForward();

    const Matrix4x4& GetEyeMatrix() {
        return eyeCollider_->GetWorldMatrix();
    };
    WorldTransform& GetEyeWorldTransform() {
        return eyeCollider_->GetWorldTransform();
    }

    WorldTransform& GetBodyWorldTransform() {
        return aniObject_->GetWorldTransform();
    }
    void SetBodyRotate(const Vector3& rotate) { aniObject_->SetRotate(rotate); }
    void SetBodyScale(const Vector3& scale) { aniObject_->SetScale(scale); }
    HPs* GetHpsPtr() { return &characterState_.hps; }

    void OnCollision(Collider* collider)override;
    void OnCollisionEnemy(const int hitPoint = 10);

    EyePosition* GetEyeCollider() {
        return eyeCollider_.get();
    }

    const bool& IsDead() { return characterState_.isDead; }
    const float& GetZoomTimer() { return zoomTimer_; }
    //レイスプライトクラスを包含する
    RaySprite* GerRaySprite() { return raySprite_.get(); };
    Matrix4x4* GetHandMatrixPtr() { return &handMatrix_; }
private:

    void UpdateRay();
    void Move();
    void Jump();
    void Zoom();
    void Thermography();
    void MouseLook();
private:

#pragma region//カメラ情報
    //カメラ速度
    float cameraSpeed_ = 1.0f;
    //カメラ回転Y
    float cameraRotateY_ = 0.0f;
    //カメラ回転X
    float cameraRotateX_ = 0.0f;
    //ズームタイマー
    float zoomTimer_ = 1.0f;
    //ズームフラグ
    bool isZoom_ = false;
    //ズーム開始タイマー
    float zoomStartTimer_ = 0.0f;
    std::unique_ptr<EyePosition>eyeCollider_ = nullptr;
#pragma endregion

    //レイスプライトクラスを包含する
    std::unique_ptr<RaySprite> raySprite_ = nullptr;
    //キャラクター状態
    CharacterState characterState_;
    //無敵フラグ
    bool isInvincible_ = false;
    //ヒットタイマー
    float hitTimer_ = 0.0f;
    //床との衝突
    bool isFloorHit_ = false;

#pragma region//ジャンプ
    //ジャンプフラグ
    bool isJump_ = false;
    //ジャンプスピード
    const float kJumpSpeed_ = 0.3125;
#pragma endregion

#pragma region//サーモグラフィー
    //サーモグラフィー有効フラグ
    bool isThermography_ = false;
    //サーモグラフィー終了取得
    bool isThermographyEnd_ = false;
    //サーモグラフィー度
    float thermography_ = 0.0f;
#pragma endregion

#pragma region//物理情報
    //速度
    Vector3 velocity_;
    //速度倍率を格納
    float speed_;
#pragma endregion

    //音声が鳴るタイマー
    float soundTimer_ = 0.0f;
    //モデル
    Model* model_;
    //アニメーション付きオブジェクト
    std::unique_ptr<AnimationObject3d> aniObject_ = nullptr;

#pragma region//jointの行列を格納
    Matrix4x4 headMatrix_{};
    Matrix4x4 handMatrix_{};
#pragma endregion
};

