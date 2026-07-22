#pragma once

#include <list>
#include"Sphere.h"
#include"AABB.h"

class Collider;
class Camera;
namespace ColliderWorldPos {
    Sphere GetSphereWorldPos(Collider* sphere);
    AABB GetAABBWorldPos(Collider* aabb);
}


/// @brief 衝突マネージャ
class CollisionManager {
public:
    /// @brief コライダーを追加する
    /// @param collider コライダー
    void AddCollider(Collider* collider) { colliders_.emplace_back(collider); }

    template <typename Container>
    void AddColliders(const Container& colliders)
    {
        for (auto& collider : colliders) {
            colliders_.emplace_back(collider);
        }
    }
    /// @brief コライダーをクリアする
    void ClearColliders() { colliders_.clear(); }

    /// @brief 衝突判定と応答
    void CheckAllCollisions();
    /// @brief 全てを描画する
    /// @param camera カメラ
    void DrawAllCollider(Camera* camera);
    /// @brief コライダーをオブジェクト位置に移動するための更新処理　デバック用
    void UpdateAllCollider();
    /// @brief 全てのコライダーを描画するフラグの取得
    /// @return フラグ
    const bool GetIsDraw() const { return isDraw_; };
    /// @brief 全てのコライダーを描画するフラグをセットする
    /// @param flag フラグをセット
    void SetIsDraw(const bool flag) { isDraw_ = flag; };
    void Finalize() { ClearColliders(); }
private:
    // コライダーのリスト
    std::list<Collider*> colliders_; 
    //全てのコライダー描画フラグ
    bool isDraw_ = false;
private:
    void  CheckCollisionPair(Collider* a, Collider* b);
    void OnCollision(Collider* a, Collider* b);
    /// @brief コライダー2つの衝突判定と応答
    /// @param colliderA コライダーA
    /// @param colliderB コライダーB
    /// @param score スコアポインタ
    void CheckCollisionSpherePair(Collider* colliderA, Collider* colliderB);

    /// @brief コライダー2つの衝突判定と応答
    /// @param colliderA コライダーA
    /// @param colliderB コライダーB
    /// @param score スコアポインタ
    void CheckCollisionAABBPair(Collider* colliderA, Collider* colliderB);

    /// @brief コライダー2つの衝突判定と応答
    /// @param colliderA コライダーA
    /// @param colliderB コライダーB
    /// @param score スコアポインタ
    void CheckCollisionSphereAABBPair(Collider* colliderA, Collider* colliderB);

};