#include "Easing.h"
#include <cmath>

float EaseInOutCubic(const float& x) { return x < 0.5f ? 4 * x * x * x : 1.0f - std::powf(-2.0f * x + 2, 3) / 2.0f; }
float EaseCubic(float x) {
    return 3.0f * x * x - 2.0f * x * x * x;
}

float EaseInOutBack(const float& x){

const float c1 = 1.70158f;
const float c2 = c1 * 1.525f;

return x < 0.5
  ? (std::powf(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
  : (std::powf(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}