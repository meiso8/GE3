#pragma once
#include "Sphere.h"
#include"Plane.h"
#include"Segment.h"
#include"Ray.h"
#include"Line.h"
#include"Triangle.h"
#include"AABB.h"

/// @brief 球と球の衝突判定
/// @param s1 球1
/// @param s2 球2
/// @return 当たったかどうか
bool IsCollision(const Sphere& s1, const Sphere& s2);

/// @brief 球と平面の衝突判定
/// @param sphere 球
/// @param plane 平面
/// @return 当たったかどうか
bool IsCollision(const Sphere& sphere, const Plane& plane);

/// @brief 線分と平面の衝突判定
/// @param segment  線分
/// @param plane 平面
/// @return 当たったかどうか
bool IsCollision(const Segment& segment, const Plane& plane);

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

/// @brief AABBと線分の衝突判定
/// @param aabb AABB
/// @param segment 線分
/// @return 当たったかどうか
bool IsCollision(const AABB& aabb, const Segment& segment);