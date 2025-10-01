#include"Collision.h"
#include"Length.h"
#include"Dot.h"
#include"Cross.h"
#include"Normalize.h"
#include"Multiply.h"
#include<algorithm>


bool IsCollision(const Sphere& s1, const Sphere& s2) {

    //2つの急の中心点間距離を求める 
    float distance = Length({ s2.center- s1.center });

    if (distance <= s1.radius + s2.radius) {
        return true;
    }

    return false;
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
    //仮の中身

    float distance = Dot(sphere.center, plane.normal) - plane.distance;

    if (abs(distance) <= sphere.radius) {
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
    Vector3 closestPoint(
        std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
        std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
        std::clamp(sphere.center.z, aabb.min.z, aabb.max.z));

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
};