#pragma once

#include"WorldTransform.h"
#include"Object3d.h"
#include"Circle.h"
#include"CharacterState.h"
#include"AABB.h"
#include"Line.h"
#include"Collider.h"
#include"RaySprite.h"
#include"EyeCollider.h"

class Model;
class Camera;
enum LightMode;
class CircleMesh;
class CubeMesh;
class Sprite;

class Player :public Collider
{
public:
    void OnCollision(Collider* collider)override;
    Player();
    void Init();

    void Draw(Camera& camera, const LightMode& lightType);
    void DrawRaySprite();
    void Update();


    void UpdateRay();
    void Move();
    void Jump();
    void Zoom();

    void LookBack();
    void Thermography();
    void MouseLook();

    Vector3& GetForward();


    const Matrix4x4& GetEyeMatrix() {
        return eyeCollider_->GetWorldMatrix();
    };
    WorldTransform& GetEyeWorldTransform() {
        return eyeCollider_->GetWorldTransform();
    }
    Vector3& GetBodyPos() {
        return bodyPos_.worldTransform_.translate_;
    };

    WorldTransform& GetBodyWorldTransform() {
        return bodyPos_.worldTransform_;
    }
    void SetBodyPos(const Vector3& pos) { bodyPos_.worldTransform_.translate_ = pos; };
    void SetBodyRotate(const Vector3& rotate){ bodyPos_.worldTransform_.rotate_ = rotate; }
    void SetBodyScale(const Vector3& scale) { bodyPos_.worldTransform_.scale_ = scale; }
    HPs* GetHpsPtr() { return &characterState_.hps; }

    EyeCollider* GetEyeCollider() {
        return eyeCollider_.get();
    }
    float cameraSpeed_ = 1.0f;

    float zoomTimer_ = 1.0f;
    bool isZoom_ = false;
    float zoomStartTimer_ = 0.0f;
    std::unique_ptr<RaySprite> raySprite_ = nullptr;
    const bool& IsDead() { return characterState_.isDead; }

private:
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

    Object3d bodyPos_;

    std::unique_ptr<EyeCollider>eyeCollider_ = nullptr;
    float cameraRotateY_ = 0.0f;
    float cameraRotateX_ = 0.0f;

    CharacterState characterState_;

    void OnCollisionEnemy();
};

