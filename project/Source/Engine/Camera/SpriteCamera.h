#pragma once
#include"Matrix4x4.h"

class SpriteCamera
{
private:
    static Matrix4x4 viewProjectionMat_;

    SpriteCamera() = default;
    ~SpriteCamera() = default;
    SpriteCamera(SpriteCamera&) = delete;
    SpriteCamera& operator=(SpriteCamera&) = delete;
public:

   static void Initialize(const float& width, const float& height);
    static Matrix4x4& GetViewProjectionMatrix() {
        return viewProjectionMat_;
    };
};

