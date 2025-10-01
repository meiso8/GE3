#include "Transpose.h"

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {

    Matrix4x4 result;

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            result.m[row][column] = m.m[column][row];
        }
    }

    return result;
};