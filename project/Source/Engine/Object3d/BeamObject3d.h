#pragma once
#include<Object3d.h>
#include<wrl.h>
#include<d3d12.h>

struct Point
{
    float3 startPos;
    float padding1;
    float3 endPos;
    float padding2;
};


class BeamObject3d:public Object3d
{
public:
    void Draw(Camera& camera, const BlendMode& blendMode = kBlendModeAdd, const CullMode& cullMode = kCullModeNone, const MaskMode maskMode = kZero, const bool usePSOKey = true, const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX)override;
};

