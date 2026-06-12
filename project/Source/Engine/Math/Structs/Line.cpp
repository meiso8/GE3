#include"Line.h"
#include"AABB.h"
#include<algorithm>

bool RayIntersectsAABB(const Ray& ray, const AABB& box, float& tMin, float& tMax) {
  
    tMin = 0.0f;
    tMax = std::numeric_limits<float>::max();

    float minVal = 0.0f;
    float maxVal = 0.0f;
    float origin = 0.0f;
    float dir = 0.0f;

    for (int i = 0; i < 3; i++) {

        if (i == 0) {
            minVal = box.min.x;
            maxVal = box.max.x;
            origin = ray.origin.x;
            dir = ray.diff.x;
        }
        if (i == 1) {
            minVal = box.min.y;
            maxVal = box.max.y;
            origin = ray.origin.y;
            dir = ray.diff.y;
        }
        if (i == 2) {
            minVal = box.min.z;
            maxVal = box.max.z;
            origin = ray.origin.z;
            dir = ray.diff.z;
        }

        if (std::abs(dir) < 1e-6f) {
            // レイが軸に平行
            if (origin < minVal || origin > maxVal) {
                return false;
            }
        } else {
            float t1 = (minVal - origin) / dir;
            float t2 = (maxVal - origin) / dir;

            if (t1 > t2) std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) {
                return false;
            }
        }
    }

    return true;
}
