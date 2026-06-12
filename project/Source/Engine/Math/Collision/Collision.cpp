#define NOMINMAX
#include"Collision.h"
#include "Circle.h"
#include<algorithm>

Vector3 ClosestPoint(const Vector3& point, const Plane& plane) {
    return point - Distance(point, plane) * plane.normal;
}

Vector3 ClosestPoint(const Vector3& point, const AABB& aabb) {
    Vector3 result;
    result.x = std::clamp(point.x, aabb.min.x, aabb.max.x);
    result.y = std::clamp(point.y, aabb.min.y, aabb.max.y);
    result.z = std::clamp(point.z, aabb.min.z, aabb.max.z);
    return result;
}




Vector3 ClosestPoint(const Segment& segment, const Plane& plane) {
    float dot = Dot(plane.normal, segment.diff);
    if (dot == 0.0f) {
        return segment.origin; // 平行
    }
    float t = (plane.distance - Dot(plane.normal, segment.origin)) / dot;
    t = std::clamp(t, 0.0f, 1.0f);
    return segment.origin + t * segment.diff;
}

Vector3 ClosestPoint(const Segment& segment, const AABB& aabb) {
    Vector3 tMin{
        (aabb.min.x - segment.origin.x) / segment.diff.x,
        (aabb.min.y - segment.origin.y) / segment.diff.y,
        (aabb.min.z - segment.origin.z) / segment.diff.z
    };
    Vector3 tMax{
        (aabb.max.x - segment.origin.x) / segment.diff.x,
        (aabb.max.y - segment.origin.y) / segment.diff.y,
        (aabb.max.z - segment.origin.z) / segment.diff.z
    };
    Vector3 tNear{
        std::min(tMin.x, tMax.x),
        std::min(tMin.y, tMax.y),
        std::min(tMin.z, tMax.z)
    };
    float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
    tmin = std::clamp(tmin, 0.0f, 1.0f);
    return segment.origin + tmin * segment.diff;
}

Vector3 ClosestPoint(const Segment& segment1, const Segment& segment2) {
    Vector3 d1 = segment1.diff;
    Vector3 d2 = segment2.diff;
    Vector3 r = segment1.origin - segment2.origin;
    float a = Dot(d1, d1);
    float e = Dot(d2, d2);
    float f = Dot(d2, r);
    float s, t;
    if (a <= 1e-6f && e <= 1e-6f) {
        s = t = 0.0f;
        return segment1.origin;
    }
    if (a <= 1e-6f) {
        s = 0.0f;
        t = std::clamp(f / e, 0.0f, 1.0f);
    } else {
        float c = Dot(d1, r);
        if (e <= 1e-6f) {
            t = 0.0f;
            s = std::clamp(-c / a, 0.0f, 1.0f);
        } else {
            float b = Dot(d1, d2);
            float denom = a * e - b * b;
            if (denom != 0.0f) {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            } else {
                s = 0.0f;
            }
            t = (b * s + f) / e;
            if (t < 0.0f) {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }
    return segment1.origin + d1 * s;
}

Vector3 Normal(const Vector3& point, const AABB& aabb) {
    Vector3 closestPoint = ClosestPoint(point, aabb);
    Vector3 normal = Normalize(closestPoint - point);
    if (IsZero(normal)) {
        Vector3 center = (aabb.min + aabb.max) * 0.5f;
        Vector3 toMin = point - aabb.min;
        Vector3 toMax = aabb.max - point;
        float minDist = std::min({ toMin.x, toMin.y, toMin.z, toMax.x, toMax.y, toMax.z });
        if (minDist == toMin.x) {
            normal = { -1.0f, 0.0f, 0.0f };
        } else if (minDist == toMin.y) {
            normal = { 0.0f, -1.0f, 0.0f };
        } else if (minDist == toMin.z) {
            normal = { 0.0f, 0.0f, -1.0f };
        } else if (minDist == toMax.x) {
            normal = { 1.0f, 0.0f, 0.0f };
        } else if (minDist == toMax.y) {
            normal = { 0.0f, 1.0f, 0.0f };
        } else if (minDist == toMax.z) {
            normal = { 0.0f, 0.0f, 1.0f };
        }
    }
    return normal;
}


Vector3 Normal(const Segment& segment, const AABB& aabb) {
    Vector3 closestPoint = ClosestPoint(segment, aabb);
    return Normal(ClosestPoint(closestPoint, aabb), aabb);
}


float Distance(const Vector3& point, const Plane& plane) {
    return std::abs(Dot(point, plane.normal) - plane.distance);
}

float Distance(const Vector3& point, const AABB& aabb) {
    Vector3 closestPoint = ClosestPoint(point, aabb);
    return Length(closestPoint - point);
}


float Distance(const Segment& segment, const Plane& plane) {
    Vector3 closestPoint = ClosestPoint(segment, plane);
    return Distance(closestPoint, plane);
}

float Distance(const Segment& segment, const AABB& aabb) {
    Vector3 closestPoint = ClosestPoint(segment, aabb);
    return Distance(closestPoint, aabb);
}


float Distance(const Segment& segment1, const Segment& segment2) {
    //線分同士の最近接点を求める
    Vector3 closestPoint1 = ClosestPoint(segment1, segment2);
    Vector3 closestPoint2 = ClosestPoint(segment2, segment1);
    //最近接点同士の距離を求める
    return Length(closestPoint2 - closestPoint1);
}

float PenetrationDepth(const Sphere& sphere, const Plane& plane) {
    float distance = Distance(sphere.center, plane);
    return sphere.radius - distance;
}

float PenetrationDepth(const Sphere& sphere, const AABB& aabb) {
    float distance = Distance(sphere.center, aabb);
    return sphere.radius - distance;
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {

    //2つの急の中心点間距離を求める 
    float distance = Length({ s2.center - s1.center });

    if (distance <= s1.radius + s2.radius) {
        return true;
    }

    return false;
}

bool IsCollision(const Ray& ray, const Sphere& sphere,Vector3& Q1,Vector3& Q2) {

   //計算でrayを原点に移動するため　球の中心も移動する
   Vector3 P2 = sphere.center - ray.origin;

   float A = Dot(ray.diff, ray.diff);
   float B = Dot(ray.diff, P2);
   float C = Dot(P2, P2) - sphere.radius * sphere.radius;

   if (A == 0.0f) {
       //レイの長さが0
       return false;
   }

   //解の公式のルートの中の部分を計算する
   float s = B * B - A * C;

   if (s < 0.0f) {
       //実数解無し？
       //衝突していない
       return false;
   }

   //ルートにする
   s = sqrtf(s);

   float a1 = (B - s) / A;
   float a2 = (B + s) / A;

   if (a1 < 0.0f || a1 < 0.0f) {
       //レイの反対で衝突
       return false;
   }

   //貫通点を代入する
    Q1 = ray.origin + a1 *ray.diff;
    Q2 = ray.origin + a2 *ray.diff;

    return true;
}

float Distance(const Circle& p1, const Circle& p2) {
    return Length({ p1.center - p2.center });
}

bool IsCollisionInCircleLine(const Circle& smallCircle, const Circle& largeCircle)
{
    //2つの急の中心点間距離を求める 
    if (Distance(smallCircle, largeCircle) >= largeCircle.radius - smallCircle.radius) {
        return true;
    }

    return false;
}



//線分　Segment
bool IsCollision(const Segment& segment, const Plane& plane) {

    //まずは垂直判定を行うために、法線と線の内積を求める
    float dot = Dot(plane.normal, segment.diff);

    //垂直=平行であるので、衝突しているはずがない
    if (dot == 0.0f) {
        return false;
    }

    //tを求める
    float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

    if ((t >= 0.0f) && (t <= 1.0f)) {
        return true;
    }

    return false;

}

//半直線Ray
bool IsCollision(const Ray& ray, const Plane& plane) {

    //まずは垂直判定を行うために、法線と線の内積を求める
    float dot = Dot(plane.normal, ray.diff);

    //垂直=平行であるので、衝突しているはずがない
    if (dot == 0.0f) {
        return false;
    }

    //tを求める
    float t = (plane.distance - Dot(ray.origin, plane.normal)) / dot;

    if (t == -1) {
        return false;
    }

    if (t == 2) {
        return true;
    }

    return false;
}

//直線Line
bool IsCollision(const Line& line, const Plane& plane) {

    //まずは垂直判定を行うために、法線と線の内積を求める
    float dot = Dot(plane.normal, line.diff);

    //垂直=平行であるので、衝突しているはずがない
    if (dot == 0.0f) {
        return false;
    }

    //tを求める
    float t = (plane.distance - Dot(line.origin, plane.normal)) / dot;

    if (t == -1) {
        return true;
    }

    if (t == 2) {
        return true;
    }

    return false;
}

//三角形と線分
bool IsCollision(const Triangle& triangle, const Segment& segment) {


    //三角形から作られた平面と線との衝突判定から媒介変数tを求める


    //平面作成のためにベクトルを計算する
    Vector3 v01 = Vector3(triangle.vertices[1]) - Vector3(triangle.vertices[0]);
    Vector3 v12 = Vector3(triangle.vertices[2]) - Vector3(triangle.vertices[1]);

    //三角形から平面を作成する
    Plane plane;
    plane.normal = Normalize(Cross(v01, v12));
    plane.distance = Dot(triangle.vertices[0], plane.normal);

    //tを求める
    float dot = Dot(plane.normal, segment.diff);
    float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

    //線の種類によって平面と衝突しているかどうかは媒介変数tによって判断し、衝突している場合は衝突点pが三角形の内側かを判断する

    if ((t >= 0.0f) && (t <= 1.0f)) {
        //衝突していたら残りのベクトルを計算
        Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];

        //衝突点pは p = o+t*b
        Vector3 p = segment.origin + Multiply(t, segment.diff);

        Vector3 v1p = p - triangle.vertices[1];
        Vector3 v2p = p - triangle.vertices[2];
        Vector3 v0p = p - triangle.vertices[0];

        //各頂点が結んだベクトルと、頂点の衝突点pを結んだベクトルのクロス積を取る
        Vector3 cross01 = Cross(v01, v1p);
        Vector3 cross12 = Cross(v12, v2p);
        Vector3 cross20 = Cross(v20, v0p);

        //全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
        if (Dot(cross01, plane.normal) >= 0.0f &&
            Dot(cross12, plane.normal) >= 0.0f &&
            Dot(cross20, plane.normal) >= 0.0f
            ) {
            //衝突
            return true;
        }

    }


    return false;
}

//AABBとAABBの衝突判定
bool IsCollision(const AABB& a, const AABB& b) {

    if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&//x軸
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&//y軸
        (a.min.z <= b.max.z && a.max.z >= b.min.z)) {
        return true;
    }

    return false;

}

//AABBと球の衝突判定
bool IsCollision(const AABB& aabb, const Sphere& sphere) {

    //球の中心とAABBとの最近接点を求める
    //球の中心の座標をAABBの[min,max]内にclampすればそれが最近接点となる
    //clampとは範囲内に値を収めることである
    Vector3 closestPoint = ClosestPoint(sphere.center, aabb);

    //球の中心がAABB内部の場合は、最近接点の球の中心となる

    //最近接点と球の中心との距離を求める
    float distance = Length(closestPoint - sphere.center);
    //距離が半径よりも小さければ衝突　    //球の中心と最近接点との距離が球の半径よりも短ければ衝突
    if (distance <= sphere.radius) {
        //衝突
        return true;
    }

    return false;

};

//AABBとpointの衝突判定
bool IsCollision(const AABB& aabb, const Vector3& point) {

    //球の中心とAABBとの最近接点を求める
    //球の中心の座標をAABBの[min,max]内にclampすればそれが最近接点となる
    //clampとは範囲内に値を収めることである
    Vector3 closestPoint = ClosestPoint(point, aabb);

    //球の中心がAABB内部の場合は、最近接点の球の中心となる

    //最近接点と球の中心との距離を求める
    float distance = Length(closestPoint - point);
    //距離が半径よりも小さければ衝突　    //球の中心と最近接点との距離が球の半径よりも短ければ衝突
    if (distance <= 0.1f) {
        //衝突
        return true;
    }

    return false;

};


bool IsCollision(const AABB& aabb, const Segment& segment) {


    float txMin = (aabb.min.x - segment.origin.x) / segment.diff.x;
    float txMax = (aabb.max.x - segment.origin.x) / segment.diff.x;

    float tyMin = (aabb.min.y - segment.origin.y) / segment.diff.y;
    float tyMax = (aabb.max.y - segment.origin.y) / segment.diff.y;

    float tzMin = (aabb.min.z - segment.origin.z) / segment.diff.z;
    float tzMax = (aabb.max.z - segment.origin.z) / segment.diff.z;

    float tNearX = std::min(txMin, txMax);
    float tNearY = std::min(tyMin, tyMax);
    float tNearZ = std::min(tzMin, tzMax);

    float tFarX = std::max(txMin, txMax);
    float tFarY = std::max(tyMin, tyMax);
    float tFarZ = std::max(tzMin, tzMax);

    float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
    float tmax = std::min(std::min(tFarX, tFarY), tFarZ);


    if (tmin <= tmax) {

        if (tmin >= 0.0f && tmin <= 1.0f && tmax >= 0.0f && tmax <= 1.0f) {
            return true;
        }

    }

    return false;
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
    return Distance(sphere.center, plane) <= sphere.radius;
}


bool IsCollision(const Vector2& p, const Rect& r) {
    return (p.x >= r.left && p.x <= r.left + r.width &&
        p.y >= r.top && p.y <= r.top + r.height);
}



