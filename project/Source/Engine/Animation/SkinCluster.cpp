#include"SkinCluster.h"
#include"ModelData.h"
#include"Bone.h"
#include"DirectXCommon.h"
#include"SrvDescriptorHeap.h"
#include<dxgi1_6.h>
#include <algorithm>
#include"MakeMatrix.h"


SrvDescriptorHeap* Skin::srvDescriptorHeap_ = nullptr;

SkinCluster Skin::CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData)
{
    SkinCluster skinCluster;

    //palette用のResourceの確保
    skinCluster.paletteResource = DirectXCommon::CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
    WellForGPU* mappedPalette = nullptr;
    skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
    skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };//spanを使ってアクセスするようにする
    //インデックスを格納
    skinCluster.paletteSrvIndex = srvDescriptorHeap_->Allocate();
    skinCluster.paletteSrvHandle.first = srvDescriptorHeap_->GetCPUDescriptorHandle(skinCluster.paletteSrvIndex);
    skinCluster.paletteSrvHandle.second = srvDescriptorHeap_->GetGPUDescriptorHandle(skinCluster.paletteSrvIndex);

    //palette用のsrvを作成
    srvDescriptorHeap_->CreateSRVforStructuredBuffer(skinCluster.paletteSrvIndex, skinCluster.paletteResource.Get(), UINT(skeleton.joints.size()), sizeof(WellForGPU));

    //influence用のResourceを確保
    skinCluster.influenceResource = DirectXCommon::CreateBufferResource(sizeof(VertexInfluence) * modelData.vertices.size());
    VertexInfluence* mappedInfluence = nullptr;
    skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
    std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());//0埋め。wightを0にしておく
    skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };

    //influence用のVBVを作成
    skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
    skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
    skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

    //inverseBindMatrixを格納する場所を作成して、単位行列で埋める
    skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
    //シーケンスの初期化に便利なアルゴリズム　for分の役割
    std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4);

    //ModelDataのSkinCluster情報を解析してInfluenceの中身を埋める
    for (const auto& jointWeight : modelData.skinClusterData) {
        auto it = skeleton.jointMap.find(jointWeight.first);
        if (it == skeleton.jointMap.end()) {
            continue;
        }

        // (*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
        skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
        for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
            auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];//該当のvertexIndexのinfluence情報を参照しておく
            for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
                //空いてるところに入れる
                if (currentInfluence.weights[index] == 0.0f) {
                    currentInfluence.weights[index] = vertexWeight.weight;
                    currentInfluence.jointIndices[index] = (*it).second;
                    break;
                }

            }
        }

    }



    return skinCluster;
}

void Skin::UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton)
{

    for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
        assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
        skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
            skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
        skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
            Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
    }
}

void Skin::SetSrvDescriptorHeap(SrvDescriptorHeap* srvDescriptorHeap)
{
    srvDescriptorHeap_ = srvDescriptorHeap;
    assert(srvDescriptorHeap);
}
