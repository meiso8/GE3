#include"Normalize.h"
#include"Length.h"

Vector3 Normalize(const Vector3& v) {
    float length = Length(v);
    return { v.x / length,v.y / length,v.z / length };
};

Vector2 Normalize(const Vector2& v) {
    float length = Length(v);
    return { v.x / length,v.y / length };
};
//
//template<typename Type> Type Normalize(Type& v) {
//
//    float length = Length(v)
//
//};