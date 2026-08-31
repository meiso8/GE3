#include "AnimationObject3d.h"
#include"TimeManager.h"
#include"Model.h"
#include"ModelData.h"
#include"Lights/PointLightManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"MakeMatrix.h"
#include"SkinningModel.h"
#include"Bone.h"
#include<algorithm>
#include"CbvSrvUavDescriptorHeap.h"
#include"DirectXCommon.h"
#include"Log.h"
#include "ComputeShaderPSO/ComputeShaderPSO.h"
#include"ObjectManager/ObjectManager.h"
AnimationObject3d::AnimationObject3d() {
    animationTime_ = 0.0f;
    worldMatrix_ = MakeIdentity4x4();
    //スキニングモデルを内部で宣言する
    skinningModel_ = std::make_unique<SkinningModel>();
    //一旦ここでヒープを入れておく
    skinningModel_->SetDescriptorHeap(cbvSrvUavDescriptorHeap_);
#ifdef _DEBUG
    debugBone_ = std::make_unique<DebugBone>();
#endif
}

AnimationObject3d::~AnimationObject3d()
{
#ifdef _DEBUG
    if (debugBone_) {
        debugBone_->Finalize();
    }
#endif
}


void AnimationObject3d::Initialize()
{
    animationTime_ = 0.0f;
    worldTransform_.Initialize();
}

void AnimationObject3d::Update()
{
    WorldTransformUpdate(worldTransform_);
    UpdateAnimation();

}

void AnimationObject3d::InitTime()
{
    animationTime_ = 0.0f;
}

std::map<std::string, Animation>& AnimationObject3d::GetAnimations()
{
    return skinningModel_->GetModelData()->animations_;
}

Matrix4x4 AnimationObject3d::GetWorldJointMatrix(const std::string name)
{
    auto* matrix = skinningModel_->GetJointMatrix(name);
   
    //行列があったら
    if (matrix) {
       return *matrix* worldMatrix_;
    }

    return MakeIdentity4x4();
}

void AnimationObject3d::UpdateAnimation()
{
    ModelData* modelData = skinningModel_->GetModelData();
    Skeleton* skeleton = skinningModel_->GetSkeleton();
    SkinCluster* skinCluster = skinningModel_->GetSkinCluster();

    assert(modelData);
    assert(skeleton);
    assert(skinCluster);

    auto& animations = modelData->animations_;

    assert(&animations);

    Animation* animation = nullptr;
    if (animations.contains(currentAnimation_)) {
        animation = &animations.at(currentAnimation_);
    } else {

        for (auto& [name, findAnimation] : animations) {
            animation = &findAnimation;
            //最新のアニメーションの名前を入れる
            currentAnimation_ = name;
            break;
        }
    }
  
    if (animation) {
        //アニメーションがあったら

        //アニメーションの更新を行って、骨ごとのLocal情報を更新する
        ApplyAnimation(*skeleton, *animation, animationTime_);
        //現在の骨ごとのLocal情報を基にSkeletonSpaceの情報を更新する
       Bone::UpdateSkeleton(*skeleton);
        //SkeletonSpaceの情報を基に、SkinClusterのMatrixPaletteを更新する
       Skin::UpdateSkinCluster(*skinCluster, *skeleton);
    }

    if (isSkinning_) {
        worldMatrix_ = worldTransform_.matWorld_;
    } else {
        worldMatrix_ = worldTransform_.matWorld_* modelData->rootNode.localMatrix;
    }

#ifdef _DEBUG
    debugBone_->Update(worldTransform_.matWorld_);
#endif
}

void AnimationObject3d::UpdateAniTimer(const bool& isLoop)
{
    animationTime_ += TimeManager::DeltaTime();

    ModelData* modelData = skinningModel_->GetModelData();

    Animation* animation = nullptr;

    auto& animetions = modelData->animations_;

    if (animetions.contains(currentAnimation_)) {
        animation = &animetions.at(currentAnimation_);
    } else {

        for (auto& [name, findAnimation] : animetions) {
            animation = &findAnimation;
            //最新のアニメーションの名前を入れる
            currentAnimation_ = name;
            break;
        }
    }

    if (isLoop) {
        animationTime_ = std::fmod(animationTime_, animation->duration);
    } else {
        animationTime_ = std::clamp(animationTime_, 0.0f, animation->duration);
    }
}
bool AnimationObject3d::IsAnimEnd()
{
    ModelData* modelData = skinningModel_->GetModelData();

    auto& animetions = modelData->animations_;

    if (animetions.contains(currentAnimation_)) {
        return  animationTime_ == animetions.at(currentAnimation_).duration;
    }
    //もしアニメーションが見つからなかったらfalseで返す
    LogFile::Log("not found animetion : IsAnimEnd return false");
    return false;
}
void AnimationObject3d::SetModelAndLoadAnimation(Model* model)
{
    assert(model);

    skinningModel_->SetModel(model);

#ifdef _DEBUG
    debugBone_->Create(*skinningModel_->GetSkeleton());
#endif

}

void AnimationObject3d::Draw(
    const BlendMode& blendMode, 
    const CullMode& cullMode,
    const MaskMode maskMode,
    const bool usePSOKey,
    const TextureFactory::Handle skyBoxTexture, 
    const TextureFactory::Handle dissolveTexture,
    const bool useObjectManagerDraw)
{

    blendMode_ = blendMode;
    cullMode_ = cullMode;
    maskMode_ = maskMode;
    usePSOKey_ = usePSOKey;
    skyBoxTexture_ = skyBoxTexture;
    dissolveTexture_ = dissolveTexture;

    if (useObjectManagerDraw) {
        ObjectManager::GetInstance()->SetDrawObject(material_->color.w, this);
    }
}

void AnimationObject3d::DrawCommand(Camera& camera)
{

    transformationMatrixResource_.data->World = worldMatrix_;
    transformationMatrixResource_.data->WorldInverseTranspose = Transpose(Inverse(worldMatrix_));
    transformationMatrixResource_.data->WVP = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());

    //スキンクラスター
    auto* skinCluster = skinningModel_->GetSkinCluster();
    auto* csResource = skinningModel_->GetCSResource();

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = csResource->outputVertexResource_.resource.Get();
    // 前回の描画終わり（または初期状態）のステート
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    // CSで書き込むためのステート
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList_->ResourceBarrier(1, &barrier);

    commandList_->SetComputeRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::CS_SKINNING));
    commandList_->SetPipelineState(ComputeShaderPSO::GetInstance()->GetSkinningPSO().Get());
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(0, skinCluster->paletteSrvIndex, commandList_);
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(1, csResource->inputVertexResource_.srvIndex, commandList_);
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(2, skinCluster->influenceSrvIndex, commandList_);
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(3, csResource->outputVertexResource_.uavIndex, commandList_);
    commandList_->SetComputeRootConstantBufferView(4, csResource->skinningInformationResource_.GetGPUVirtualAddress());

    //ComputeShaderの実行
    commandList_->Dispatch(UINT(skinningModel_->GetModelData()->vertices.size() + 1023) / 1024, 1, 1);

    // ====================================================================
   // 2. Compute Shader実行後：UAVステート -> 頂点バッファステートへ戻す
   // ====================================================================
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    commandList_->ResourceBarrier(1, &barrier);

    if (skinningModel_) {
        skinningModel_->SetRootSignatureAndGraphicsPipeline(commandList_, blendMode_, cullMode_, maskMode_, usePSOKey_);

        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_.GetGPUVirtualAddress());
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE], commandList_);
        //cameraのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(3, camera.GetResource()->GetGPUVirtualAddress());
        //ID
        commandList_->SetGraphicsRootConstantBufferView(4, idResource_.GetGPUVirtualAddress());
        //ライト
        DirectionalLightManager::SetGraphicsRootConstantBufferView(5, commandList_);
        //expansionのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(6, expansionResource_.GetGPUVirtualAddress());
        //WaveのSRVの場所を設定
        commandList_->SetGraphicsRootShaderResourceView(7, waveResource_.GetGPUVirtualAddress());
        //ライトのCBufferの場所を設定
          //PointLightのDescriptorTableの設定をする
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(8, PointLightManager::GetSrvIndex(), commandList_);
        //SpotLightのDescriptorTableの設定をする
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(9, SpotLightManager::GetSrvIndex(), commandList_);
        //環境テクスチャの設定
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(10, Texture::GetSRVHandle(skyBoxTexture_), commandList_);
        //MeltData
        commandList_->SetGraphicsRootConstantBufferView(11, meltResource_.GetGPUVirtualAddress());
        //SkyBox
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(12, Texture::GetSRVHandle(dissolveTexture_), commandList_);

        //ここでテクスチャの設定をする
        MeshDraw();
    }


#ifdef _DEBUG
    debugBone_->Draw(camera);
#endif

}

void AnimationObject3d::MeshDraw()
{
    commandList_->IASetPrimitiveTopology(skinningModel_->GetTopology());
    commandList_->IASetVertexBuffers(0, 1, &skinningModel_->GetVertexBufferView());

    //モデルデータの取得
    auto* modelData = skinningModel_->GetModelData();
    DrawModel(modelData);
}

