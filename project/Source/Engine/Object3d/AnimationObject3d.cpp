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

    Animation* animation = nullptr;
    if (animations_.contains(currentAnimation_)) {
        animation = &animations_.at(currentAnimation_);
    } else {

        for (auto& [name, findAnimation] : animations_) {
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
        UpdateSkinCluster(*skinCluster, *skeleton);
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
    animationTime_ += Time::DeltaTime();

    Animation* animation = nullptr;

    if (animations_.contains(currentAnimation_)) {
        animation = &animations_.at(currentAnimation_);
    } else {

        for (auto& [name, findAnimation] : animations_) {
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

    if (animations_.contains(currentAnimation_)) {
        return  animationTime_ == animations_.at(currentAnimation_).duration;
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
    //試しにここでセットしてみる　
    animations_ = AnimationManager::GetAnimations(model->GetModelData()->filePath);
}

void AnimationObject3d::Draw(Camera& camera,  const BlendMode& blendMode, const CullMode& cullMode, const MaskMode maskMode,const bool usePSOKey,const TextureFactory::Handle skyBoxTexture)
{
    transformationMatrixData_->World = worldMatrix_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldMatrix_));
    transformationMatrixData_->WVP = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());

    if (skinningModel_) {

        auto* commandlist = DirectXCommon::GetCommandList();
        skinningModel_->PreDraw(commandlist, blendMode, cullMode,maskMode,usePSOKey);
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandlist->SetGraphicsRootConstantBufferView(0, materialResource_->GetMaterialResource()->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        //方向ライト
        DirectionalLightManager::SetGraphicsRootConstantBufferView(3);
        ////拡散反射テクスチャ
        //SrvManager::SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE]);
        //timeのSRVの場所を設定
        commandlist->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
        //expansionのCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());
        //cameraのCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(6, camera.GetResource()->GetGPUVirtualAddress());
        //ライトの場所を設定
        PointLightManager::SetGraphicsRootDescriptorTable(7);
        SpotLightManager::SetGraphicsRootDescriptorTable(8);
        SrvManager::SetGraphicsRootDescriptorTable(9, Texture::GetSRVHandle(skyBoxTexture));
        //ここでテクスチャの設定をする
        skinningModel_->Draw(commandlist);
    }

#ifdef _DEBUG
    debugBone_->Draw(camera);
#endif

}

