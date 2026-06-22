#pragma once
#include"Vector3.h"
#include"Quaternion/Quaternion.h"

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