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
#include"SrvDescriptorHeap.h"
#include"DirectXCommon.h"
#include"Log.h"

AnimationObject3d::AnimationObject3d() {
    animationTime_ = 0.0f;
    worldMatrix_ = MakeIdentity4x4();
    //スキニングモデルを内部で宣言する
    skinningModel_ = std::make_unique<SkinningModel>();
#ifdef _DEBUG
    debugBone_ = std::make_unique<DebugBone>();
#endif
}

AnimationObject3d::~AnimationObject3d()
{
    Object3d::~Object3d();
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

void AnimationObject3d::Draw(Camera& camera,  const BlendMode& blendMode, const CullMode& cullMode, const MaskMode maskMode,const bool usePSOKey,const TextureFactory::Handle skyBoxTexture)
{
    transformationMatrixData_->World = worldMatrix_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldMatrix_));
    transformationMatrixData_->WVP = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());

    if (skinningModel_) {

      
        skinningModel_->SetRootSignatureAndGraphicsPipeline(commandList_, blendMode, cullMode,maskMode,usePSOKey);
       
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        srvDescriptorHeap_->SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE], commandList_);
        //cameraのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(3, camera.GetResource()->GetGPUVirtualAddress());
        //ID
        commandList_->SetGraphicsRootConstantBufferView(4, idResource_->GetGPUVirtualAddress());
        //ライト
        DirectionalLightManager::SetGraphicsRootConstantBufferView(5, commandList_);
        //expansionのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(6, expansionResource_->GetGPUVirtualAddress());
        //WaveのSRVの場所を設定
        commandList_->SetGraphicsRootShaderResourceView(7, waveResource_->GetGPUVirtualAddress());
        //ライトのCBufferの場所を設定
          //PointLightのDescriptorTableの設定をする
        srvDescriptorHeap_->SetGraphicsRootDescriptorTable(8, PointLightManager::GetSrvIndex(), commandList_);
        //SpotLightのDescriptorTableの設定をする
        srvDescriptorHeap_->SetGraphicsRootDescriptorTable(9, SpotLightManager::GetSrvIndex(), commandList_);

        srvDescriptorHeap_->SetGraphicsRootDescriptorTable(10, Texture::GetSRVHandle(skyBoxTexture), commandList_);
        //ここでテクスチャの設定をする
        MeshDraw();
    }

#ifdef _DEBUG
    debugBone_->Draw(camera);
#endif

}

void AnimationObject3d::MeshDraw()
{

    commandList_->IASetPrimitiveTopology(primitive_->GetTopology());
    //スキンクラスター
    auto* skinCluster = skinningModel_->GetSkinCluster();

    D3D12_VERTEX_BUFFER_VIEW vbvs[2] = { primitive_->GetVertexBufferView(), skinCluster->influenceBufferView };
    commandList_->IASetVertexBuffers(0, 2, vbvs);//VBVを設定

    //cameraのCBufferの場所を設定 paletteResource 
    srvDescriptorHeap_->SetGraphicsRootDescriptorTable(11, skinCluster->paletteSrvIndex,commandList_);
 
    //モデルデータの取得
    auto* modelData = skinningModel_->GetModelData();
    DrawModel(modelData);

}

