#pragma once
#include "Vector3.h"
#include <cstdint>
#include"AABB.h"
#include"Object3d.h"
#include"Primitive.h"
#include"Line.h"
#include "CollisionConfig.h"

class Camera;

struct CollisionInfo {
	bool collided;
	Vector3 normal;//法線
	float penetration;//めり込み量
};

CollisionInfo GetCollisionInfo(const Sphere& sphere, const AABB& AABB);
CollisionInfo GetCollisionInfo(const AABB& a, const AABB& b);
void ResolveCollision(Vector3& pos, Vector3& velocity, const CollisionInfo& info);

/// @brief 衝突判定オブジェクト
class Collider {
public:

	enum ColliderType {
		kSphere,
		kAABB,
		kRay
	};

private:
	float radius_ = 1.0f;	// 衝突半径
	AABB aabb_;
	Ray ray_;

	std::string colliderName_ = "Collider";
	uint32_t collisionAttribute_ = 0xffffffff;	// 衝突属性
	uint32_t collisionMask_ = 0xffffffff;		// 衝突マスク

	ColliderType type_ = ColliderType::kSphere;
	CollisionInfo collisionInfo_;
	//新しく中心と座標を追加
	Vector3 center_ = {0.0f,0.0f,0.0f};
	Matrix4x4* worldMat_ = nullptr;

	Vector3 tempWorldTransform_ = {0.0f};
	bool isCalculatedThisFrame_ = false; // 今フレーム計算済みかどうかのフラグ
#ifdef _DEBUG
	//デバック用
	std::unique_ptr<Primitive>debugMesh_ = nullptr;
	//位置
	std::unique_ptr<Object3d> object3d_ = nullptr;
#endif // DEBUG
public:
	Collider();
	virtual ~Collider();

	void InitCalcuatedTisFrameFlag();
	const std::string GetColliderName() { return colliderName_; };
	void SetColliderName(const std::string& colliderName) { colliderName_ = colliderName; }
	/// @brief 衝突時コールバック関数
	virtual void OnCollision(Collider* collider) {
		(void)collider;
		//コライダーを赤くする
		OnCollisionCollider();

	};
	/// @brief ワールド座標を取得する関数の作成
	/// @return 
	const Vector3& CalculateWorldPos();
	/// @brief 中心点を設定する
	/// @param center 
	void SetCenter(const Vector3& center) {
		center_ = center;
	};
	/// @brief ワールド座標を設定する
	/// @param worldMat 
	void SetWorldMatrix(Matrix4x4& worldMat) {
		assert(&worldMat);
		worldMat_ = &worldMat;
	};
	/// @brief ワールドトランスフォーム用のマトリックスセット関数
	/// @param worldTransform 
	void SetWorldMatrix(WorldTransform& worldTransform) {
		assert(&worldTransform);
		SetWorldMatrix(worldTransform.matWorld_);
	}

	void SetWorldMatrix(Object3d& object3d) {
		SetWorldMatrix(object3d.GetWorldTransform());
	}
	/// @brief 衝突半径を取得する
	/// @return 衝突半径
	float GetRadius() const { return radius_; }

	/// @brief 衝突半径を設定する
	/// @param radius 衝突半径
	void SetRadius(float radius) { 
		type_ = ColliderType::kSphere;
#ifdef _DEBUG

		if (debugMesh_) {
			debugMesh_->Create(PrimitiveGenerator::CreateSphere({ .center = {0.0f,0.0f,0.0f}, .radius = radius }));
			object3d_->SetMeshAndMaterial(debugMesh_.get());
		}

#endif // DEBUG
		radius_ = radius; }

	const AABB& GetAABB() const { return aabb_; }
	void SetAABB(const AABB& aabb) {

		type_ = ColliderType::kAABB;

#ifdef _DEBUG
		if (debugMesh_) {
			debugMesh_->Create(PrimitiveGenerator::CreateCube(aabb));
			object3d_->SetMeshAndMaterial(debugMesh_.get());
		}
#endif // DEBUG

		aabb_ = aabb;
	};

	ColliderType GetType() const { return type_; }

	/// @brief 衝突属性を取得する
	/// @return 衝突属性
	uint32_t GetCollisionAttribute() const { return collisionAttribute_; }

	/// @brief 衝突属性を設定する
	/// @param attribute 衝突属性
	void SetCollisionAttribute(uint32_t attribute) { collisionAttribute_ = attribute; }

	/// @brief 衝突マスクを取得する
	/// @return 衝突マスク
	uint32_t GetCollisionMask() const { return collisionMask_; }

	/// @brief 衝突マスクを設定する
	/// @param mask 衝突マスク
	void SetCollisionMask(uint32_t mask) { collisionMask_ = mask; }
	void ColliderUpdate();
	void ColliderDraw(Camera& camera);
	void OnCollisionCollider();
	void SetCollisionInfo(const CollisionInfo& info) { collisionInfo_ = info; };
	CollisionInfo& GetCollisionInfo() {
		return collisionInfo_;
	}
};

