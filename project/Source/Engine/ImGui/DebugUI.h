#pragma once

#include"ImGuiClass.h"
#include"Vector3.h"
#include"Vector4.h"
#include"WorldTransform.h"
#include<list>
#include"Transform.h"
#include"Balloon.h"
#include"Wave.h"
#include"Melt.h"
#include <functional>
#include<memory>
#include<unordered_map>
#include"Matrix/CameraMatrix.h"
#include"PostEffectMaterial/PostEffectMaterial.h"

enum BlendMode;
struct AABB;
class Input;
class Sprite;
class Font;

class Model;
class Primitive;

class ParticleManager;
struct Emitter;
class Object3d;
class AnimationObject3d;

struct Material;

struct CameraData;

struct SphericalCoordinate;
struct QuaternionTransform;

struct PointLight;

struct CharacterState;
class CbvSrvUavDescriptorHeap;
class Collider;
#include "nlohmann/json.hpp"
class DebugUI
{
public:
    static void ShowMainViewPort(CbvSrvUavDescriptorHeap* srvDescriptorHeap, const uint32_t srvIndex);
    static void CheckCaracterState(CharacterState& characterState, const char* label);
    static void CheckFPS();
    static void CheckSound();
    static void CheckJsonFile();
    static nlohmann::json& FindJsonFile(std::string& tagName, bool useFilter = false, const char* containFileName = "unKnown");
    static void ShowJsonFileSaveMessage(const std::string& name);
    static void CreateJsonFile(const char* containFileName = "JsonFileName");
    static void CheckSRVIndex(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    static void CheckSRVTexture(const int srvIndex, CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    static void CheckTextures(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    static void CheckStageManager();

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
    static void CheckMeltData(Melt& melt, const char* label);

    /// @brief Object3d
    /// @param object3d Object3d
    /// @param label ラベル
    static void CheckObject3d(Object3d& object3d, const char* label= "SelectObject : ");
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

    /// @brief コライダーのチェック
    /// @param collider コライダーのアドレス
    /// @param label ラベル
    static void CheckCollider(Collider& collider, const char* label);

    /// @brief 
    /// @param worldTransform 
    /// @param label 
    static void CheckQuaternionTransform(QuaternionTransform& quaternionTransform, const char* label);


    //行列の要素を見る関数
    static void ShowMatrix4x4(const Matrix4x4& matrix, const char* label = "Matrix4x4");

    /// @brief カメラ
    /// @param camera 
    static void CheckCamera(CameraMatrix& cameraMatrix, CameraData& cameraOption, const char* label = "Camera");
    static void CheckShericalCoordinate(SphericalCoordinate& sphericalCoordinate);
    static void CheckParticle(ParticleManager* particleManager);
    static void CheckEmitter(Emitter& emitter, const char* label = "Emitter");
    /// @brief 色
    /// @param color 
    static void CheckColor(Vector4& color, const char* label);
    static void CheckObject3dMaterial(
        Vector4& color,
        int32_t& lightMode,
        float& shininess,
        float& tempereture,
        Matrix4x4& uvMatrix,
        float& environmentCoefficient,
        float& maskVal,
        Vector3& dissolveRgb,
        float& maskEdgeMin,
        float& maskEdgeMax,
        float& gamma,
        const char* label
    );
    static void CheckLightMode(int32_t& lightMode, const char* label);
    static void CheckBlendMode(BlendMode& blendMode);

    static void CheckInt(int& value, const char* label);

    static void CheckFloat(float& value, const char* label);

    static void CheckFlag(bool& flag, const char* label);
    static void SwitchFlag(bool& flag, const char* label);
    static void Button(const char* label, std::function<void()> onSwitch);
    static void CheckPostEffect();
    //AABBを調べる
    static void CheckAABB(AABB& aabb, const char* label = "AABB");

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

    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForRenderTexture& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForVignette& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForBoxFilter& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForGaussianFilter& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForLuminanceBasedOutline& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForDepthBasedOutline& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForRadialBlur& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForDissolve& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForRandom& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForThermography& material);
    static void CheckPostEffectMaterial(PostEffectMaterial::MaterialForMosaic& material);
};

