#pragma once
#include"Vector3.h"
#include"Quaternion/Quaternion.h"
#include<numbers>

namespace Math {
    const float kPi = std::numbers::pi_v<float>;
    const float kHPi = kPi * 0.5f;
    const float kQPi = kPi *0.25f;
}

struct EulerTransform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
    bool operator==(const EulerTransform& other) const {
        return scale == other.scale && rotate == other.rotate && translate == other.translate;
    }

};

struct QuaternionTransform {
    Vector3 scale;
    Quaternion rotate;
    Vector3 translate;
};