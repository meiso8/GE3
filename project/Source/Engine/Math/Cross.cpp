#include"Cross.h"

//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {

    //左手でベクトルの順に巻いて親指が向く方向
    Vector3 result;

    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;

    return result;
}
//二つ
// のベクトルが向かい合っているかは内積で判断できる
//c・(a×b)<= 0; 上記の外積と内積で判断