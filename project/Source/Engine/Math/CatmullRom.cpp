#include "CatmullRom.h"
#include "Lerp.h"
#include <algorithm>
#include <cassert>
#include <cmath>

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {

	const float s = 0.5f;

	float t2 = t * t;
	float t3 = t2 * t;

	Vector3 e3 = -p0 + 3.0f * p1 - 3.0f * p2 + p3;
	Vector3 e2 = 2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3;
	Vector3 e1 = -p0 + p2;
	Vector3 e0 = 2.0f * p1;
	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
};

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) {

	assert(points.size() >= 4 && "制御点は4点以上必要です");

	size_t division = points.size() - 1;
	float areaWidth = 1.0f / division;

	/*float t_2 = t - areaWidth * division;*/
	float t_2 = std::fmod(t, areaWidth) / areaWidth;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	size_t index = std::min(points.size() - 1, static_cast<size_t>(t / areaWidth));
	size_t index0 = (index == 0) ? index : index - 1;
	size_t index1 = index;
	size_t index2 = std::min(points.size() - 1, index + 1);
	size_t index3 = std::min(points.size() - 1, index + 2);

	if (index == 0) {
		index0 = index1;
	}

	if (index3 >= points.size()) {
		index3 = index2;
	}

	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
};