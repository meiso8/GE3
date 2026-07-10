#pragma once
#include"ResourceManager/ResourceManager.h"

class CbvSrvUavDescriptorHeap;
class DsvDescriptorHeap;

class DepthTexture
{
public:
    void CreateResource(CbvSrvUavDescriptorHeap* srvDescriptorHeap,int32_t width, int32_t height);
   void  InitializeDepthStencilView(DsvDescriptorHeap* dsvDescriptorHeap);
   void DebugViewer(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
   ID3D12Resource* GetResource() { return depthTextureData_.Get(); };
   uint32_t GetSRVIndex() { return depthTextureData_.srvIndex; }
private:
    TextureResource depthTextureData_;
};

