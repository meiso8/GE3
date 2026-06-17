#pragma once

#include"ImGuiClass.h"
#include"Vector3.h"
#include"Vector4.h"
#include"WorldTransform.h"
#include<list>
#include"Transform.h"
#include"Balloon.h"
#include"Wave.h"
#include <functional>
#include<memory>
#include<unordered_map>

enum BlendMode;
class Input;
class Sprite;
class Font;
class SphereMesh;
class Camera;
class Model;
class ParticleManager;
class ParticleEmitter;
class Object3d;
class AnimationObject3d;
struct Material;
struct PointLight;
struct QuaternionTransform;
class MeshCommon;
//struct Damage;
struct CharacterState;


class DebugUI
{
public:
    static void CheckCaracterState(CharacterState& characterState, const char* label);
    static void CheckFPS();
    static void CheckSound();
    static void CheckJsonFile();
    static void CheckSRVIndex();
    static void CheckSRVTexture(const int srvIndex);
    static void CheckTextures();
  /*  static void CheckDamageStruct(Damage& damage, const char* label);*/
    /// @brief 入力
    /// @param input 
    static void CheckInput();
    static void CheckXInput(const int& num);

    static void CheckLights();

    /// @brief モデル
    /// @param model
    /// @param label 
    static void CheckModels();
    /// @brief スプライト
    /// @param sprite 
    static void CheckSprite(Sprite& sprite, const char* label);
    /// @brief Font
/// @param Font 
    static void CheckFont(Font& sprite, const char* label);
    static void CheckBalloonData(Balloon& balloon);
    static void CheckWaveData(Wave& wave, const char* label);

    /// @brief Object3d
    /// @param object3d Object3d
    /// @param label 
    static void CheckObject3d(Object3d& object3d, const char* label);
    /// @brief ワールドトランスフォーム
    /// @param worldTransform 
    static void CheckWorldTransform(WorldTransform& worldTransform, const char* label);
    /// @brief ワールドマトリックス
    /// @param scale 
    /// @param rotate 
    /// @param translate 
    /// @param label 
    static void CheckTransform(EulerTransform& transform, const char* label);

    static void CheckTransforms(Vector3& scale, Vector3& rotate, Vector3& translate, const char* label);

    /// @brief 
    /// @param worldTransform 
    /// @param label 
    static void CheckQuaternionTransform(QuaternionTransform& quaternionTransform, const char* label);


    //行列の要素を見る関数
    static void ShowMatrix4x4(const Matrix4x4& matrix, const char* label = "Matrix4x4");

    /// @brief カメラ
    /// @param camera 
    static void CheckCamera(Camera& camera, const char* label = "Camera");

    static void CheckParticle(ParticleEmitter& particleEmitter, const char* label);
    /// @brief 色
    /// @param color 
    static void CheckColor(Vector4& color, const char* label);
    static void CheckMaterial(Material& material, const char* label);
    static void CheckLightMode(int32_t& lightMode, const char* label);
    static void CheckBlendMode(BlendMode& blendMode);

    static void CheckInt(int& value, const char* label);

    static void CheckFloat(float& value, const char* label);

    static void CheckFlag(bool& flag, const char* label);
    static void SwitchFlag(bool& flag, const char* label);
    static void Button(const char* label, std::function<void()> onSwitch);



private:
    /// @brief 光源
/// @param directionalLights 
/// @param lightType 
    static void CheckDirectionalLight();
    /// @brief 光源
/// @param CheckSpotLight 
/// @param lightType 
    static void CheckSpotLight();
    static void CheckPointLightData();
};

