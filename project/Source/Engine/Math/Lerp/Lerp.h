#pragma once

#include"Vector3.h"

#include<vector>

class Camera;

template <typename T> T Lerp(T start, T end, float t) {
    return static_cast<T>(start * (1.0f - t) + end * t);
}

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);


template <typename T> T Bezier(const T& p0, const T& p1, const T& p2, float t) {

    //制御点p0,p1を線形補間
    T p0p1 = Lerp(p0, p1, t);
    //制御点p1,p2を線形補間
    T p1p2 = Lerp(p1, p2, t);
    //制御点p0p1,p1p2をさらに線形補間
    T p = Lerp(p0p1, p1p2, t);

    return static_cast<T>(p);

};

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);
