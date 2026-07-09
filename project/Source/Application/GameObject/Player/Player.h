#pragma once

#include"WorldTransform.h"
#include"Object3d.h"
#include"Circle.h"
#include"CharacterState.h"
#include"AABB.h"
#include"Line.h"
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

    Vector3& GetForward();

    const Matrix4x4& GetEyeMatrix() {
        return eyeCollider_->GetWorldMatrix();
    };
    WorldTransform& GetEyeWorldTransform() {
        return eyeCollider_->GetWorldTransform();
    }

    WorldTransform& GetBodyWorldTransform() {
        return bodyPos_->GetWorldTransform();
    }
    void SetBodyRotate(const Vector3& rotate){ bodyPos_->SetRotate(rotate); }
    void SetBodyScale(const Vector3& scale) { bodyPos_->SetScale(scale); }
    HPs* GetHpsPtr() { return &characterState_.hps; }

    void OnCollision(Collider* collider)override;
    void OnCollisionEnemy(const int hitPoint = 10);

    EyeCollider* GetEyeCollider() {
        return eyeCollider_.get();
    }

    const bool& IsDead() { return characterState_.isDead; }
    const float& GetZoomTimer() { return zoomTimer_; }
    //レイスプライトクラスを包含する
    RaySprite* GerRaySprite() { return raySprite_.get(); };
private:

    void UpdateRay();
    void Move();
    void Jump();
    void Zoom();
    void LookBack();
    void Thermography();
    void MouseLook();
private:
    //カメラ速度
    float cameraSpeed_ = 1.0f;
    //ズームタイマー
    float zoomTimer_ = 1.0f;
    //ズームフラグ
    bool isZoom_ = false;
    //ズーム開始タイマー
    float zoomStartTimer_ = 0.0f;
    //レイスプライトクラスを包含する
    std::unique_ptr<RaySprite> raySprite_ = nullptr;
    //無敵
    bool isInvincible_ = false;
    //床との衝突
    bool isFloorHit_ = false;
    const float kJumpSpeed_ = 0.3125;
    float hitTimer_ = 0.0f;
    float endRotateY_ = 0.0f;
    float startRotateY = 0.0f;
    float lookBackTime_ = 1.0f;
    bool isLookBackEnd_ = false;

    bool isThermography_ = false;

    bool isThermographyEnd_ = false;
    float thermography_ = 0.0f;

    bool isLookBack_ = false;
    bool isJump_ = false;
    //AABB
    AABB localAabb_;
    Vector3 velocity_;
    float kSpeed_;

    float soundTimer_ = 0.0f;
    Model* model_;

    std::unique_ptr<Object3d> bodyPos_  = nullptr;
    std::unique_ptr<EyeCollider>eyeCollider_ = nullptr;
    float cameraRotateY_ = 0.0f;
    float cameraRotateX_ = 0.0f;

    CharacterState characterState_;

};

