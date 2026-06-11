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
    void Create();
    void SetPos(const Vector3 start, const Vector3 end);
    void SetStartPos(const Vector3 start);
    void SetEndPos(const Vector3 end);
    void Draw(Camera& camera, const BlendMode& blendMode = kBlendModeNormal, const CullMode& cullMode = kCullModeBack, const MaskMode maskMode = kAll, const bool usePSOKey = false, const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX)override;

    Point& GetPoint() { return *pointData_; }

private:
    void CreatePointData();

    Microsoft::WRL::ComPtr <ID3D12Resource> pointDataResource_ = nullptr;
    Point* pointData_ = nullptr;

};

