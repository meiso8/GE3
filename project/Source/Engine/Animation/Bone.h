#pragma once
#include"Transform.h"
#include"Matrix4x4.h"
#include<string>
#include<vector>
#include<cstdint>
#include<optional>
#include<map>
#include"Node.h"

#include "LineObject3d/LineObject3d.h"

#include<tuple>
#include<memory>

class Camera;
struct Joint {
    QuaternionTransform transform;
    Matrix4x4 localMatrix;
    Matrix4x4 skeletonSpaceMatrix;//skeletonSpaceでの変換行列
    std::string name;//名前
    std::vector<int32_t> children;
    int32_t index;
    //ある型にbool値で無効か有効かの情報を追加した型　ポインタ以外にnullptrを追加するともいえる
    std::optional<int32_t>parent;
};

struct Skeleton {
    int32_t root;//RootJointのIndex
    std::map<std::string, int32_t> jointMap;//Joint名とIndexとの辞書
    std::vector<Joint> joints;//所属しているジョイント
};

namespace Bone{
    Skeleton CreateSkeleton(const Node& rootNode);
    Joint* GetJoint(const std::string name, Skeleton& skeleton);
    Matrix4x4* GetJointMatrix(const std::string name, Skeleton& skeleton);

    int32_t CreateJoint(
        const Node& node,
        const std::optional<int32_t>& parent,
        std::vector<Joint>& joints);

    void UpdateSkeleton(Skeleton& skeleton);
}


class DebugBone {
private:
    std::vector < std::unique_ptr<LineObject3d>>bones_;
    Skeleton* skeleton_ = nullptr;
public:
    void Create(Skeleton& skeleton);
    void Update(const Matrix4x4& parentMatrix);
    void Draw(Camera& camera);
 
};