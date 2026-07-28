#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "Object3d.h"
#include"ObjectCommandManager.h"
#include "nlohmann/json.hpp"
#include"Collider.h"
class CollisionManager;
class CbvSrvUavDescriptorHeap;
class RenderTexture;

namespace ImGuizmo {
    enum OPERATION : int;
    enum MODE : int;
}
class ObjectManager
{
public:
    //インスタンスの取得
    static ObjectManager* GetInstance();
    
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;

    // オブジェクトの登録（登録時に自動で一意のIDが割り当てられます）
    void RegisterObject(Object3d* gameObject);
    // オブジェクトの削除
    void UnregisterObject(Object3d* gameObject);
    // IDからオブジェクトを即座に検索（ピッキング用）
    Object3d* FindObjectByID(uint32_t id);
    // オブジェクトチェック
    void ClickObject(Camera& camera);
    void SetRenderTextureForModel(RenderTexture* renderTexture);
    // シーン切り替え時などの全クリア
    void Clear();
    //初期化
    void Initialize();
    //コマンドリストとSRVDescriptorHeapの設定
    void SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    //更新
    void Update();
    void CheckCollision(CollisionManager* collisionManager);
    //描画
    void Draw(Camera& camera);
    //終了処理
    void Finalize();
private:      
    void Save();
    void SetName();
    void CreateObject();
    bool SelectObject(Camera& camera);
    bool UpdateImGuizmo(Camera& camera, Object3d* object3d);
    bool UpdateImGuizmoForCollider(Camera& camera, Collider* collider, const Matrix4x4& objectMat);
    void SelectMovement(ImGuizmo::MODE& currentMode,ImGuizmo::OPERATION& currentOperation, bool& snap);
    void ColliderCheck(Camera& camera);
    ObjectManager() = default;
    ~ObjectManager() = default;
private:
    RenderTexture* renderTexture_ = nullptr;

    ObjectCommandManager objectCommandManager_;
    std::vector<Object3d*> objects_;
    std::unordered_map<uint32_t, Object3d* > idMap_;
    uint32_t nextID_ = 1; // 1から順にAllocateしていく
    uint32_t clickedID_ = 0;
    bool editCollider_ = false;
    Object3d* colliderObjectPtr_ = nullptr;
    /// @brief オブジェクトエディターのファイル名を設定しておく
    const std::string kJsonFileName_ = "objectEditor";
    /// @brief jsonファイル名
    std::string jsonFileName_ = kJsonFileName_;
    //オブジェクト
    std::vector<std::unique_ptr<Object3d>> createObjects_;
    //コライダーマップ
    std::unordered_map<Object3d*, std::unique_ptr<Collider>>colliderMaps_;
};

