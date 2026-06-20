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
    for (auto& value : bones_) {
        value->Draw(camera, true);
    }
}

void DebugBone::Create(Skeleton& skeleton)
{
    skeleton_ = &skeleton;
    bones_.clear();
    assert(!skeleton_->joints.empty());

    for (Joint& joint : skeleton_->joints) {

        std::unique_ptr<LineObject3d> object3d = std::make_unique<LineObject3d>();
        object3d = std::make_unique<LineObject3d>();
        object3d->Create(Vector3{ 0.0f,0.0f,0.0f }, joint.transform.translate);
        object3d->SetColor({ 1.0f,0.0f,0.0f,1.0f });
        bones_.push_back(std::move(object3d));
    }
}

#include"DebugUI.h"


void CheckJoint(Joint& joint, std::vector<Joint>& joints)
{

#ifdef USE_IMGUI
    ImGui::PushID(joint.index);

    // ツリーのノードを作成（名前を表示）
    // 子がいない場合は葉ノード（Bullet）にするオプション
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (joint.children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    }

    if (ImGui::TreeNodeEx(joint.name.c_str(), flags)) {
        // ノードが開かれている間だけ中身を表示
        DebugUI::CheckQuaternionTransform(joint.transform, "Transform");
        DebugUI::ShowMatrix4x4(joint.localMatrix, "localMat");
        DebugUI::ShowMatrix4x4(joint.skeletonSpaceMatrix, "skeletonSpaceMat");
        ImGui::Separator();

        // 子要素を再帰呼び出し
        for (auto& child : joint.children) {
            CheckJoint(joints[child], joints);
        }

        ImGui::TreePop(); // 開いたツリーを閉じる（超重要！）
    }

    ImGui::PopID();
#endif
}
void DebugBone::Update(const Matrix4x4& parentMatrix)
{
    for (int i = 0; i < bones_.size(); ++i) {

        auto& joint = skeleton_->joints[i];

        if (joint.parent.has_value()) {
            int32_t parentIndex = joint.parent.value();
            Vector3 parentPos = skeleton_->joints[parentIndex].transform.translate;
            bones_[i]->SetVertex(joint.transform.translate, parentPos);

        } else {
            bones_[i]->SetVertex(joint.transform.translate, skeleton_->joints[0].transform.translate);
        }

        bones_[i]->worldTransform_.matWorld_ = parentMatrix * joint.skeletonSpaceMatrix;
    }

#ifdef USE_IMGUI

    ImGui::Begin("Bones");

        CheckJoint(skeleton_->joints[0], skeleton_->joints);

    ImGui::End();
#endif
}
