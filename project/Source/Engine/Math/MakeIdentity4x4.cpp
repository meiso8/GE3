#include "MakeIdentity4x4.h"

Matrix4x4 MakeIdentity4x4() {

    Matrix4x4 result;

    result = { 1.0f,0.0f,0.0f,0.0f,
               0.0f,1.0f,0.0f,0.0f,
               0.0f,0.0f,1.0f,0.0f,
               0.0f,0.0f,0.0f,1.0f,
    };

    return result;

};