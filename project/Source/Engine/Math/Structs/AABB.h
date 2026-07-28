#pragma once
#include"Vector3.h"

//Axis Aligned Bounding Box
struct AABB {
    Vector3 min;
    Vector3 max;
 
};

namespace Math {
    Vector3 AABBCenter(const AABB& aabb);
    Vector3 AABBSize(const AABB& aabb);
}

AABB operator+(const AABB& aabb1, const AABB& aabb2);