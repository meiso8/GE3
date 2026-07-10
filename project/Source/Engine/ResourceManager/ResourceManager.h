#pragma once
#include<d3d12.h>
#include<wrl.h>
#include<string>
#include"Vector4.h"
#include"SrvDescriptorHeap.h"
#include<stdint.h>


namespace ResourceFactory {
    /// @brief BufferResourceの作成関数
    /// @param sizeInBytes 
    /// @return BufferResource
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResourceForUAV(
        size_t sizeInBytes);
    /// @brief BufferResourceの作成関数
    /// @param sizeInBytes 
    /// @return BufferResource
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        size_t sizeInBytes);
    /// @brief ReadbackBufferResource作成関数
    /// @param sizeInBytes 
    /// @return ReadbackBufferResource
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateReadbackBufferResource(size_t sizeInBytes);
    /// @brief テクスチャリソースの作成関数
    /// @param metadata metadataの指定
    /// @return テクスチャリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
    /// @brief レンダーテクスチャの作成
    /// @param device デバイス
    /// @param width 幅
    /// @param height 高さ
    /// @param format フォーマット
    /// @param clearColor クリアカラー
    /// @return レンダーテクスチャ
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(
        uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

    /// @brief StencilTextureの作成関数　奥行き
    /// @param device 
    /// @param width 
    /// @param height 
    /// @return StencilTexture
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencileTextureResource(
        int32_t width,
        int32_t height);

    /// @brief テクスチャデータの転送関数
    /// @param texture 
    /// @param mipImages 
    /// @return テクスチャデータ中間リソース
    [[nodiscard]]
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(
        ID3D12GraphicsCommandList* commandList,
        const Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
        const DirectX::ScratchImage& mipImages);
}

struct IResource {
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    IResource() = default;
    ID3D12Resource* Get() { return resource.Get(); }
    void Reset();
    void UnMap();
    void SetName(const LPCWSTR& label);
};

struct TextureResource :public IResource {
    uint32_t srvIndex = 0;
};


template <typename T>
struct Resource :public IResource {
    T* data = nullptr;
    void CreateBufferResource(const LPCWSTR& label, size_t sizeInBytes = sizeof(T));
    HRESULT Map();
    HRESULT Map(T* ptr);

};


template <typename T>
struct CResource :public Resource<T> {
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();
};

template <typename T>
struct UAVResource :public CResource<T> {
    uint32_t uavIndex = 0;
    void CreateBufferResourceForUAV(const LPCWSTR& label, size_t sizeInBytes = sizeof(T));
    void Allocate(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    void CreateUAV(CbvSrvUavDescriptorHeap* srvDescriptorHeap, UINT numElements, UINT structureByteStride = sizeof(T));
};


template <typename T>
struct SRVResource :public Resource<T> {
    uint32_t srvIndex = 0;

    void Allocate(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    void CreateSRVforStructuredBuffer(CbvSrvUavDescriptorHeap* srvDescriptorHeap, UINT numElements, UINT structureByteStride = sizeof(T));
};

struct IntermediateResource :public IResource {
    void UploadTextureData(const LPCWSTR& label, ID3D12GraphicsCommandList* commandList,ID3D12Resource* ohterResource, const DirectX::ScratchImage& mipImages);
};

struct SRVTextureResource :public IResource {
    uint32_t srvIndex = 0;
    DirectX::TexMetadata metadata{};
    IntermediateResource intermediateResource;
    void Allocate(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    void CreateTextureResource(const LPCWSTR& label);
    void UploadTextureData(const LPCWSTR& label, ID3D12GraphicsCommandList* commandList, const DirectX::ScratchImage& mipImages);
    void CreateSRVforTexture(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
};


template<typename T>
inline void Resource<T>::CreateBufferResource(const LPCWSTR& label, size_t sizeInBytes)
{
    if (IResource::resource) {
        return;
    }

    IResource::resource = ResourceFactory::CreateBufferResource(sizeInBytes);
    IResource::resource->SetName(label);
}

template<typename T>
inline HRESULT Resource<T>::Map()
{
    //書き込むためのアドレスを取得
    return IResource::resource->Map(0, nullptr, reinterpret_cast<void**>(&data));
}

template<typename T>
inline HRESULT Resource<T>::Map(T* ptr)
{
    return IResource::resource->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
}

template<typename T>
inline void SRVResource<T>::Allocate(CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    //まだないとき
    if (srvIndex == 0) {
        srvIndex = srvDescriptorHeap->Allocate();
    }


}

template<typename T>
inline void SRVResource<T>::CreateSRVforStructuredBuffer(CbvSrvUavDescriptorHeap* srvDescriptorHeap, UINT numElements, UINT structureByteStride)
{
    srvDescriptorHeap->CreateSRVforStructuredBuffer(
        srvIndex,
        IResource::Get(),
        numElements,
        structureByteStride
    );
}

template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS CResource<T>::GetGPUVirtualAddress()
{
    return IResource::Get()->GetGPUVirtualAddress();
}

template<typename T>
inline void UAVResource<T>::CreateBufferResourceForUAV(const LPCWSTR& label, size_t sizeInBytes)
{

    if (IResource::resource) {
        return;
    }

    IResource::resource = ResourceFactory::CreateBufferResourceForUAV(sizeInBytes);
    IResource::resource->SetName(label);

}

template<typename T>
inline void UAVResource<T>::Allocate(CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{    //まだないとき
    if (uavIndex == 0) {
        uavIndex = srvDescriptorHeap->Allocate();
    }

}

template<typename T>
inline void UAVResource<T>::CreateUAV(CbvSrvUavDescriptorHeap* srvDescriptorHeap, UINT numElements, UINT structureByteStride)
{
    srvDescriptorHeap->CreateUAV(
        uavIndex,
        IResource::Get(),
        numElements,
        structureByteStride
    );
}


inline void SRVTextureResource::Allocate(CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    //まだないとき
    if (srvIndex == 0) {
        srvIndex = srvDescriptorHeap->Allocate();
    }
}


inline void SRVTextureResource::CreateTextureResource(const LPCWSTR& label)
{
    if (IResource::resource) {
        return;
    }
    IResource::resource = ResourceFactory::CreateTextureResource(metadata);
    IResource::resource->SetName(label);
}

inline void SRVTextureResource::UploadTextureData(const LPCWSTR& label, ID3D12GraphicsCommandList* commandList, const DirectX::ScratchImage& mipImages)
{
    intermediateResource.UploadTextureData(label, commandList, IResource::Get(), mipImages);
}

inline void SRVTextureResource::CreateSRVforTexture(CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    srvDescriptorHeap->CreateSRVforTexture(
        srvIndex,
        IResource::Get(),
        metadata
    );

}
