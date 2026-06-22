#include"Matrix4x4.h"

//行列の加法
Matrix4x4 Add(const Matrix4x4 &m1, const Matrix4x4 &m2) {

	Matrix4x4 result;

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}

	return result;

};

//行列の減算
Matrix4x4 Subtract(const Matrix4x4 &m1, const Matrix4x4 &m2) {

	Matrix4x4 result;

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}

	return result;

};

//行列の積
Matrix4x4 Multiply(const Matrix4x4 &m1, const Matrix4x4 &m2) {

	Matrix4x4 result;

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] =
				m1.m[row][0] * m2.m[0][column] +
				m1.m[row][1] * m2.m[1][column] +
				m1.m[row][2] * m2.m[2][column] +
				m1.m[row][3] * m2.m[3][column];
		}
	}

	return result;
};




Matrix4x4 operator+(const Matrix4x4 &m1, const Matrix4x4 &m2) { return Add(m1, m2); };
Matrix4x4 operator-(const Matrix4x4 &m1, const Matrix4x4 &m2) { return Subtract(m1, m2); };
Matrix4x4 operator*(const Matrix4x4 &m1, const Matrix4x4 &m2) { return Multiply(m1, m2); }
Matrix4x4 operator*(const Matrix4x4 &m1, float s) {
	Matrix4x4 result;
	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] * s;
		}
	}
	return result;
}

