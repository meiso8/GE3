#pragma once
#include "Sphere.h"
#include "Plane.h"
#include "Line.h"
#include "Triangle.h"
#include "AABB.h"
#include"Rect.h"
#include"Vector2.h"
struct Circle;

/// @brief 反射ベクトルを求める関数
/// @param input 入射ベクトル
/// @param normal 法線
/// @return 反射ベクトル
Vector3 Reflect(const Vector3& input, const Vector3& normal);

/// @brief 射影関数
/// @param v1 ベクトル1
/// @param v2 ベクトル2
/// @return 正射影ベクトル
Vector3 Project(const Vector3& v1, const Vector3& v2);

/// @brief 点と平面の最近接点を求める
/// @param point 点
/// @param plane 平面
/// @return 最近接点
Vector3 ClosestPoint(const Vector3& point, const Plane& plane);

/// @brief 点とAABBの最近接点を求める
/// @param point 点
/// @param aabb AABB
/// @return 最近接点
Vector3 ClosestPoint(const Vector3& point, const AABB& aabb);


/// @brief 線分と平面の最近接点を求める関数
/// @param segment 線分
/// @param plane 平面
/// @return 最近接点
Vector3 ClosestPoint(const Segment& segment, const Plane& plane);

/// @brief 線分とAABBの最近接点を求める
/// @param segment 線分
/// @param aabb AABB
/// @return 最近接点
Vector3 ClosestPoint(const Segment& segment, const AABB& aabb);

/// @brief 線分と線分の最近接点を求める
/// @param segment1 線分1
/// @param segment2 線分2
/// @return 最近接点
Vector3 ClosestPoint(const Segment& segment1, const Segment& segment2);

/// @brief 点とAABBの衝突面法線を求める
/// @param point 点
/// @param aabb AABB
/// @return 衝突面法線
Vector3 Normal(const Vector3& point, const AABB& aabb);

/// @brief 線分とAABBの衝突面法線を求める
/// @param segment 線分
/// @param aabb AABB
/// @return 衝突面法線
Vector3 Normal(const Segment& segment, const AABB& aabb);


/// @brief 点と平面の距離を求める
/// @param point 点
/// @param plane 平面
/// @return 距離
float Distance(const Vector3& point, const Plane& plane);

/// @brief 点とAABBの距離を求める
/// @param point 点
/// @param aabb AABB
/// @return 距離
float Distance(const Vector3& point, const AABB& aabb);


/// @brief 線分と平面の距離を求める関数
/// @param segment 線分
/// @param plane 平面
/// @return 距離
float Distance(const Segment& segment, const Plane& plane);

/// @brief 線分とAABBの距離を求める関数
/// @param segment 線分
/// @param aabb AABB
/// @return 距離
float Distance(const Segment& segment, const AABB& aabb);


/// @brief 線分と線分の距離を求める
/// @param segment1 線分1
/// @param segment2 線分2
/// @return 距離
float Distance(const Segment& segment1, const Segment& segment2);

/// @brief 円と円の距離を求める
/// @param p1 
/// @param p2 
/// @return 距離
float Distance(const Circle& p1, const Circle& p2);

/// @brief 球と平面の貫入量を求める
/// @param sphere 球
/// @param plane 平面
/// @return 貫入量
float PenetrationDepth(const Sphere& sphere, const Plane& plane);

/// @brief 球とAABBの貫入量を求める
/// @param sphere 球
/// @param aabb AABB
/// @return 貫入量
float PenetrationDepth(const Sphere& sphere, const AABB& aabb);

/// @brief 球と球の衝突判定
/// @param s1 球1
/// @param s2 球2
/// @return 当たったかどうか
bool IsCollision(const Sphere& s1, const Sphere& s2);

/// @brief 線分と平面の衝突判定
/// @param segment  線分
/// @param plane 平面
/// @return 当たったかどうか
bool IsCollision(const Segment& segment, const Plane& plane);

/// @brief 半直線と球の衝突判定
/// @param ray 半直線
/// @param sphere 球
/// @param Q1 貫通点1
/// @param Q2 貫通点2
/// @return 当たったかどうか
bool IsCollision(const Ray& ray, const Sphere& sphere, Vector3& Q1, Vector3& Q2);

/// @brief 半直線と平面の衝突判定
/// @param ray 半直線
/// @param plane 平面
/// @return  当たったかどうか
bool IsCollision(const Ray& ray, const Plane& plane);

/// @brief 直線と平面の衝突判定
/// @param line 直線
/// @param plane 平面
/// @return 当たったかどうか
bool IsCollision(const Line& line, const Plane& plane);

/// @brief 三角形と線分の衝突判定
/// @param triangle 三角形
/// @param segment 線分
/// @return 当たったかどうか
bool IsCollision(const Triangle& triangle, const Segment& segment);

/// @brief AABBとAABBの衝突判定
/// @param a AABB1
/// @param b AABB2
/// @return 当たったかどうか
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

/// @brief AABBと球の衝突判定
/// @param aabb AABB
/// @param sphere 球
/// @return 当たったかどうか
bool IsCollision(const AABB& aabb, const Sphere& sphere);

/// @brief AABBと点の衝突判定
/// @param aabb AABB
/// @param point 点
/// @return 当たったかどうか
bool IsCollision(const AABB& aabb, const Vector3& point);

/// @brief AABBと線分の衝突判定
/// @param aabb AABB
/// @param segment 線分
/// @return 当たったかどうか
bool IsCollision(const AABB& aabb, const Segment& segment);

/// @brief 球と平面の衝突判定
/// @param sphere 球
/// @param plane 平面
/// @return 当たったかどうか
bool IsCollision(const Sphere& sphere, const Plane& plane);


/// @brief 円が円から出たときの判定
/// @param smallCircle 
/// @param largeCircle 
/// @return 判定結果
bool IsCollisionInCircleLine(const Circle& smallCircle, const Circle& largeCircle);

//点と矩形
bool IsCollision(const Vector2& p, const Rect& r);

