#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "Object3d.h"

class ObjectManager
{
public:
    static ObjectManager* GetInstance();

    // オブジェクトの登録（登録時に自動で一意のIDが割り当てられます）
    void RegisterObject(Object3d* gameObject);

    // オブジェクトの削除
    void UnregisterObject(Object3d* gameObject);

    // IDからオブジェクトを即座に検索（ピッキング用）
    Object3d* FindObjectByID(uint32_t id);

    // 全オブジェクトの一括更新と描画
    void UpdateAll();
    void DrawAll(Camera& camera);
    void DebugAll();
    // シーン切り替え時などの全クリア
    void Clear();
private:
    ObjectManager() = default;
    ~ObjectManager() = default;

    std::vector<Object3d* > objects_;
    std::unordered_map<uint32_t, Object3d* > idMap_;
    uint32_t nextID_ = 1; // 1から順にAllocateしていく
};

