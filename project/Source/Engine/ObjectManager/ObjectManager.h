#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "Object3d.h"
#include"ObjectCommandManager.h"
#include "nlohmann/json.hpp"
class SrvDescriptorHeap;

class ObjectManager
{
public:
    //インスタンスの取得
    static ObjectManager* GetInstance();
    // オブジェクトの登録（登録時に自動で一意のIDが割り当てられます）
    void RegisterObject(Object3d* gameObject);
    // オブジェクトの削除
    void UnregisterObject(Object3d* gameObject);
    // IDからオブジェクトを即座に検索（ピッキング用）
    Object3d* FindObjectByID(uint32_t id);
    // オブジェクトチェック
    void ClickObject(Camera& camera);
    // シーン切り替え時などの全クリア
    void Clear();
    //初期化
    void Initialize();
    //コマンドリストとSRVDescriptorHeapの設定
    void SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, SrvDescriptorHeap* srvDescriptorHeap);
    //更新
    void Update();
    //描画
    void Draw(Camera& camera);
private:      
    void Save();
    void SetName();
    void CreateObject();
    bool UpdateImGuizmo(Camera& camera);

    ObjectManager() = default;
    ~ObjectManager();
private:
    ObjectCommandManager objectCommandManager_;
    std::vector<Object3d*> objects_;
    std::unordered_map<uint32_t, Object3d* > idMap_;
    uint32_t nextID_ = 1; // 1から順にAllocateしていく
    uint32_t clickedID_ = 0;
    /// @brief オブジェクトエディターのファイル名を設定しておく
    const std::string kJsonFileName_ = "objectEditor";
    /// @brief jsonファイル名
    std::string jsonFileName_ = kJsonFileName_;
    std::vector<std::unique_ptr<Object3d>> createObjects_;
};

