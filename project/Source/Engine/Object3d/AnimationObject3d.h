#pragma once
#include"Object3d.h"
#include"Quaternion/Quaternion.h"
#include"Animation/Animation.h"
#include<memory>

class DebugBone;
struct SkinCluster;
class SkinningModel;
struct ModelData;

class AnimationObject3d :public Object3d
{
public:
    AnimationObject3d();
    void Initialize()override;
    void InitTime();
    void SetModelAndLoadAnimation(Model* model);

    //オーバーライド
    void Update()override;
    void UpdateAniTimer(const bool& isLoop = true);
    bool IsAnimEnd();
    void Draw(Camera& camera, const BlendMode& blendMode = kBlendModeNormal, const CullMode& cullMode = kCullModeBack, const MaskMode maskMode = kAll,const bool usePSOKye =false, const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX)override;
    void SetSkinning(const bool& flag) { isSkinning_ = flag; }
    void SetAnimation(const std::string animName) {
        currentAnimation_ = animName;
    };
    const std::string GetCurrentAnimation() { return currentAnimation_; }
    std::map<std::string, Animation>& GetAnimations() { return animations_; };
    Matrix4x4 GetWorldJointMatrix(const std::string name);
private:
    void UpdateAnimation();
    float animationTime_ = 0.0f;
    std::string currentAnimation_ = "Idle";
    std::map <std::string, Animation> animations_;
    Matrix4x4 worldMatrix_ = { 0.0f };
    bool isSkinning_ = true;
    std::unique_ptr<SkinningModel> skinningModel_ = nullptr;
#ifdef _DEBUG
    std::unique_ptr< DebugBone> debugBone_;
#endif
};

