#pragma once

#include"ImGuiClass.h"
#include"Vector3.h"
#include"Vector4.h"
#include"WorldTransform.h"

class Model;
class Input;
class Sprite;
class SphereMesh;
class Mirror;
class Camera;
class Hammer;
class Player;
class Dove;
class FPSCounter;
struct DirectionalLight;
#include<list>

class DebugUI
{
public:

    /// @brief モデル
    /// @param model 
    void CheckModel(Model& model);
    /// @brief スプライト
    /// @param sprite 
    void CheckSprite(Sprite& sprite);
    /// @brief 球
    /// @param sphere 
    void CheckSphere(SphereMesh& sphere);
    /// @brief ワールドトランスフォーム
    /// @param worldTransform 
    void CheckWorldTransform(WorldTransform& worldTransform);
    /// @brief ワールドマトリックス
    /// @param scale 
    /// @param rotate 
    /// @param translate 
    /// @param label 
    void CheckWorldMatrix(Vector3& scale, Vector3& rotate, Vector3& translate, const char* label);
    /// @brief 入力
    /// @param input 
    void CheckInput(Input& input);
    /// @brief カメラ
    /// @param camera 
    void CheckCamera(Camera& camera);
    /// @brief 光源
    /// @param directionalLights 
    /// @param lightType 
    void CheckDirectionalLight(DirectionalLight& directionalLights, int& lightType);
    /// @brief 色
    /// @param color 
    void CheckColor(Vector4& color);

    void CheckInt(int& value);
    void CheckFPS();


};

