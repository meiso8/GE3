#include"Bone.h"
#include"MakeMatrix.h"
#include"Log.h"

Skeleton Bone::CreateSkeleton(const Node& rootNode)
{
    Skeleton skeleton;
    skeleton.root = Bone::CreateJoint(rootNode, {}, skeleton.joints);

    //名前とindexのマッピングを行いアクセスしやすくなる
    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    Bone::UpdateSkeleton(skeleton);
    return skeleton;
}

Joint* Bone::GetJoint(const std::string name, Skeleton& skeleton)
{
    if (skeleton.jointMap.contains(name)) {
        int32_t index = skeleton.jointMap.at(name);
        return &skeleton.joints[index];
    } else {
        LogFile::Log("Joint : NotFound");
        return nullptr;
    }

}

Matrix4x4* Bone::GetJointMatrix(const std::string name, Skeleton& skeleton)
{
    auto* joint = Bone::GetJoint(name, skeleton);

    if (joint) {
        return &joint->skeletonSpaceMatrix;
    }

    return nullptr;
}

//Node階層に沿ってJointを作成　再帰呼び出しにより必ず自身の親は自身よりも若いindexになるようにjointsに登録する
int32_t Bone::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
    Joint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = MakeIdentity4x4();
    joint.transform = node.transform;
    joint.index = int32_t(joints.size());//現在の登録数をインデックスとする
    joint.parent = parent;
    //SkeletonのJoint列に追加
    joints.push_back(joint);

    for (const Node& child : node.children) {
        //子jointを作成し、そのIndexを登録
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }

    return joint.index;
}

void Bone::UpdateSkeleton(Skeleton& skeleton)
{
    for (Joint& joint : skeleton.joints) {
        joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
        if (joint.parent) {
            //親がいれば親の行列を掛ける
            joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
        } else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }

}

// ===================================================================================================


void DebugBone::Draw(Camera& camera)
{
    for (auto& [joint, value] : bones_) {
        value->object3d->Draw(camera);
    }
}

void DebugBone::Create(Skeleton& skeleton)
{

    bones_.clear();
    assert(!skeleton.joints.empty());

    for (Joint& joint : skeleton.joints) {

        std::unique_ptr<BoneValue> value = std::make_unique<BoneValue>();
        value->object3d = std::make_unique<Object3d>();
        value->lineMesh = std::make_unique<LineMesh>();

        std::unique_ptr<MeshData> meshData = std::make_unique<MeshData>();
        *meshData = PrimitiveGenerator::CreateLine(Vector3{ 0.0f,0.0f,0.0f }, joint.transform.translate);
        value->lineMesh->CreateLineMesh(std::move(meshData));

        value->object3d->Create();
        value->object3d->SetMeshAndMaterial(std::move(value->lineMesh).get());

        value->object3d->Initialize();
        value->object3d->worldTransform_.matWorld_ = joint.skeletonSpaceMatrix;
        value->object3d->SetLightMode(kLightModeNone);
        value->object3d->SetColor({ 1.0f,0.0f,0.0f,1.0f });

        bones_.emplace(std::make_pair(&joint, std::move(value)));

    }

}

#include"DebugUI.h"

void DebugBone::Update(const Matrix4x4& parentMatrix)
{
    for (auto& [joint, value] : bones_) {
        //ここどうするか…
        value->lineMesh->SetVertex({ 0.0f,0.0f,0.0f }, joint->transform.translate);
        value->object3d->worldTransform_.matWorld_ = joint->skeletonSpaceMatrix * parentMatrix;
    }


#ifdef USE_IMGUI

    ImGui::Begin("Bone");
    for (auto& [joint, value] : bones_) {
        if (ImGui::TreeNode(joint->name.c_str())) {
            DebugUI::CheckQuaternionTransform(joint->transform, joint->name.c_str());
            ImGui::TreePop();
        }
    }
    ImGui::End();
#endif
}
