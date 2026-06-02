#include "Animation.h"
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
//#include<assimp/postprocess.h>
#include<cassert>
#include"Bone.h"
#include<iostream>

std::unordered_map < std::string,std::map<std::string, Animation>> AnimationManager::animations_;

AnimationManager::~AnimationManager()
{
    animations_.clear();
};


std::map<std::string, Animation>& AnimationManager::GetAnimations(const std::string& filePath) {

    if (animations_.contains(filePath)) {
        return animations_.at(filePath);
    }

    return LoadAnimationFileForFilePath(filePath);
}


std::map<std::string, Animation>& AnimationManager::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
    std::string filePath = directoryPath + "/" + filename;
    return LoadAnimationFileForFilePath(filePath);
}

std::map<std::string, Animation>& AnimationManager::LoadAnimationFileForFilePath(const std::string& filePath)
{

    if (animations_.contains(filePath)) {
        return animations_.at(filePath);
    }

    std::map<std::string, Animation> modelAnimations;


    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
    assert(scene->mNumAnimations != 0);//アニメーションがない

    for (uint32_t i = 0; i < scene->mNumAnimations;++i) {
        
        Animation animation;
        //複数対応をしていく
        aiAnimation* animationAssimp = scene->mAnimations[i];
        //mDuration : mTicksPerSecondで指定された周波数における長さ、mTicksPerSecond : 周波数
        animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換　
    
        {
            //NodeAnimatioinの解析
             //assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してnodeAnimetionの情報を取ってくる
            for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
                aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
                NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

                //translate
                for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
                    aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
                    KeyframeVector3 keyframe;
                    keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
                    keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手
                    nodeAnimation.translate.keyframes.push_back(keyframe);
                }

                //rotate
                for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
                    aiQuatKey keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
                    KeyframeQuaternion keyframe;
                    keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
                    keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z ,keyAssimp.mValue.w };//右手->左手
                    nodeAnimation.rotate.keyframes.push_back(keyframe);
                }

                //scale
                for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
                    aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
                    KeyframeVector3 keyframe;
                    keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
                    keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手
                    nodeAnimation.scale.keyframes.push_back(keyframe);
                }

            }

        }


        //ハンドルとモデルをセットにする
        modelAnimations.insert(std::make_pair(scene->mAnimations[i]->mName.C_Str(), animation));
    }

    //ハンドルとモデルをセットにする
    animations_.insert(std::make_pair(filePath, modelAnimations));

    return animations_.at(filePath);
}


// Vector3用のLerp
Vector3 Interpolate(const Vector3& a, const Vector3& b, float t) {
    return Lerp(a, b, t);
}

// Quaternion用のSlerp
Quaternion Interpolate(const Quaternion& a, const Quaternion& b, float t) {
    return Slerp(a, b, t);
}

void ApplyAnimation(Skeleton& skeleton, const Animation& animation, const float animationTime)
{

    for (Joint& joint : skeleton.joints) {
        //対象のJointのAnimationがあれば、値の適用を行う。
        if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
            const NodeAnimation& rootNodeAnimation = (*it).second;
            joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
            joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
            joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);

        }

    }

}
