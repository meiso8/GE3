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
#include"SRVmanager/SrvManager.h"

AnimationObject3d::AnimationObject3d() {
    animationTime_ = 0.0f;
    worldMatrix_ = MakeIdentity4x4();

#ifdef _DEBUG
    debugBone_ = std::make_unique<DebugBone>();
#endif
}

void AnimationObject3d::Initialize()
{
    animationTime_ = 0.0f;
    worldTransform_.Initialize();
}

void AnimationObject3d::Update()
{
    UpdateAnimation();
    WorldTransformUpdate(worldTransform_);
}

void AnimationObject3d::InitTime()
{
    animationTime_ = 0.0f;
}

void AnimationObject3d::UpdateAnimation()
{
    ModelData* boneModelData = skinningModel_->GetBoneModelData();
    Skeleton* skeleton = skinningModel_->GetSkeleton();
    SkinCluster* skinCluster = skinningModel_->GetSkinCluster();

    assert(boneModelData);
    assert(skeleton);
    assert(skinCluster);

    //アニメーションの更新を行って、骨ごとのLocal情報を更新する
    ApplyAnimation(*skeleton, animation_, animationTime_);
    //現在の骨ごとのLocal情報を基にSkeletonSpaceの情報を更新する
    UpdateSkeleton(*skeleton);
    //SkeletonSpaceの情報を基に、SkinClusterのMatrixPaletteを更新する
    UpdateSkinCluster(*skinCluster, *skeleton);

    if (isSkinning_) {
        worldMatrix_ = worldTransform_.matWorld_;
    } else {
        worldMatrix_ = boneModelData->rootNode.localMatrix * worldTransform_.matWorld_;
    }

#ifdef _DEBUG
    debugBone_->Update(worldTransform_.matWorld_);
#endif
}
void AnimationObject3d::UpdateAniTimer(const bool& isLoop)
{
    animationTime_ += kInverseFPS;
    if (isLoop) {
        animationTime_ = std::fmod(animationTime_, animation_.duration);
    } else {
        animationTime_ = std::clamp(animationTime_, 0.0f, animation_.duration);
    }
}
void AnimationObject3d::SetMeshAndData(SkinningModel* skinningModel)
{
    skinningModel_ = skinningModel;
#ifdef _DEBUG
    debugBone_->Create(*skinningModel_->GetSkeleton());
#endif
    //試しにここでセットしてみる　
    animation_ = AnimationManager::GetAnimation(skinningModel_->GetBoneModelData()->filePath);
}

void AnimationObject3d::SetAnimation(Model* model)
{
    skinningModel_->SetBoneModel(model);
#ifdef _DEBUG
    debugBone_->Create(*skinningModel_->GetSkeleton());
#endif
    //試しにここでセットしてみる　
    animation_ = AnimationManager::GetAnimation(skinningModel_->GetBoneModelData()->filePath);
}

void AnimationObject3d::SetModel(Model* model)
{
    skinningModel_->SetModel(model);
}

void AnimationObject3d::SetTextureHandle(const TextureFactory::Handle& textureHandle)
{
    skinningModel_->SetTextureHandle(textureHandle);
}


void AnimationObject3d::Draw(Camera& camera, const BlendMode& blendMode, const CullMode& cullMode,const TextureFactory::Handle skyBoxTexture)
{
    transformationMatrixData_->World = worldMatrix_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldMatrix_));
    transformationMatrixData_->WVP = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());

    if (skinningModel_) {
        skinningModel_->PreDraw(commandList_, blendMode, cullMode);
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetMaterialResource()->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        //timeのSRVの場所を設定
        commandList_->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
        //expansionのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());
        //cameraのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(6, camera.GetResource()->GetGPUVirtualAddress());
        //ライトのCBufferの場所を設定
        DirectionalLightManager::SetGraphicsRootConstantBufferView();
        PointLightManager::SetGraphicsRootDescriptorTable();
        SpotLightManager::SetGraphicsRootDescriptorTable();
        SrvManager::SetGraphicsRootDescriptorTable(10, Texture::GetHandle(skyBoxTexture));
        skinningModel_->Draw(commandList_);
    }

#ifdef _DEBUG
    debugBone_->Draw(camera);
#endif

}

