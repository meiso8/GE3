#pragma once
#include<Object3d.h>
#include<wrl.h>
#include<d3d12.h>

class BeamObject3d:public Object3d
{
public:
    ~BeamObject3d();
    void Draw(const BlendMode& blendMode = kBlendModeAdd,
        const CullMode& cullMode = kCullModeNone, 
        const MaskMode maskMode = kZero,
        const bool usePSOKey = true, 
        const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX,
        const TextureFactory::Handle dissolveTexture = TextureFactory::Handle::NOIZE0,
        const bool useObjectManagerDraw = true
    )override;
    void DrawCommand(Camera& camera)override;
};

