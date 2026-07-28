#include"AABB.h"

Vector3 Math::AABBCenter(const AABB& aabb) { return (aabb.min + aabb.max) * 0.5f; };
Vector3 Math::AABBSize(const AABB& aabb) { return aabb.max - aabb.min; };


AABB operator+(const AABB& aabb1, const AABB& aabb2)
{
    return { .min = {aabb1.min + aabb1.min}, .max = {aabb1.max + aabb2.max} };
}
