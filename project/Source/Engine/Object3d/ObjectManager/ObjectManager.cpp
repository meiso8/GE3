#include "ObjectManager.h"
#include"DebugUI.h"
#include"RenderTexture/RenderTexture.h"
#include"ImGuizmo.h"
#include"Input.h"

ObjectManager* ObjectManager::GetInstance() {
    static ObjectManager instance;
    return &instance;
}

void ObjectManager::RegisterObject(Object3d* gameObject) {
    if (!gameObject) return;
    if (gameObject->GetObjectID() != 0) {
        //あいでぃーあり
        return;
    }
    // 新しいIDを割り当てる（Allocate）
    uint32_t allocatedID = nextID_++;
    gameObject->SetObjectID(allocatedID);

    // 管理用コンテナに追加
    objects_.push_back(gameObject);
    idMap_[allocatedID] = gameObject;
}

void ObjectManager::UnregisterObject(Object3d* gameObject) {
    if (!gameObject) return;

    uint32_t id = gameObject->GetObjectID();
    idMap_.erase(id);

    auto it = std::remove(objects_.begin(), objects_.end(), gameObject);
    if (it != objects_.end()) {
        objects_.erase(it, objects_.end());
    }
}

Object3d* ObjectManager::FindObjectByID(uint32_t id) {
    auto it = idMap_.find(id);
    if (it != idMap_.end()) {
        return it->second;
    }
    return nullptr; // 見つからない（背景など）
}


void ConvertMatArray(const Matrix4x4& srcMatrix, float dstArray[16])
{
    // 行列のメモリ構造をそのまま16個のfloat配列にコピー
    std::memcpy(dstArray, &srcMatrix.m[0][0], sizeof(float) * 16);

}

void ObjectManager::ClickObject(Camera& camera)
{
#ifdef USE_IMGUI
    ImGui::Begin("Object3ds");

    clickedID_ = RenderTexture::GetInstance()->GetClickedObjectID();

    ImGui::Text("ClickedID : %d", clickedID_);

    bool isTriggerCtrlZ = Input::IsPressKey(DIK_LCONTROL) && Input::IsTriggerKey(DIK_Z);

    if (ImGui::Button("Redo")|| Input::IsPressKey(DIK_LSHIFT) && isTriggerCtrlZ) {
        objectCommandManager_.ReDo();
    } else if(ImGui::Button("Undo")|| isTriggerCtrlZ) {
        objectCommandManager_.UnDo();
    }
  

    if (clickedID_ != 0) {
        UpdateImGuizmo(camera);
    }

    ImGui::End();

#endif
}

void ObjectManager::DebugAll()
{

    for (int i = 0; i < objects_.size(); ++i) {
        std::string name = "Object:" + std::to_string(objects_[i]->GetObjectID());
        DebugUI::CheckObject3d(*objects_[i], name.c_str());
    }

}

void ObjectManager::Clear() {
    objects_.clear();
    idMap_.clear();
    nextID_ = 1; // IDのリセット
    clickedID_ = 0;
}

void ObjectManager::Initialize()
{
    objectCommandManager_.Initialize();
}

void ObjectManager::UpdateImGuizmo(Camera& camera)
{

    auto* selectedObj = FindObjectByID(clickedID_);

    if (selectedObj == nullptr) {
        return;
    }
#ifdef USE_IMGUI
    // 1. ImGuizmoのフレーム開始宣言（内部のコンテキストを初期化・更新します）
    ImGuizmo::BeginFrame();

    static ImGuizmo::MODE currentMode = ImGuizmo::LOCAL;
    static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;// TRANSLATE, ROTATE, SCALE

    if (ImGui::IsKeyPressed(ImGuiKey_T)) currentOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) currentOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_S)) currentOperation = ImGuizmo::SCALE;

    ImGuiIO& io = ImGui::GetIO();
    //画面全域に合わせる
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    float viewMat[16] = { 0.0f };
    float projectionMat[16] = { 0.0f };
    float worldMat[16] = { 0.0f };

    ConvertMatArray(camera.GetViewMatrix(), viewMat);
    ConvertMatArray(camera.GetProjectionMatrix(), projectionMat);
    ConvertMatArray(selectedObj->GetWorldMatrix(), worldMat);

    //前フレームでギズモ使用しているか
    static bool isUsingPrev = false;
    //ドラッグ開始時の状態の保存をする変数
    static EulerTransform beforeTransform;

    bool isUsingNow = ImGuizmo::IsUsing();

    //  ドラッグが始まった瞬間トリガーの状態を保存
    if (isUsingNow && !isUsingPrev) {
        beforeTransform = selectedObj->GetTransform();
    }

    if (ImGuizmo::Manipulate(
        viewMat,          // float[16]
        projectionMat,    // float[16]
        currentOperation,           // 現在の操作（移動・回転・拡大縮小）
        currentMode,                // 座標系（WORLD / LOCAL）
        worldMat,   // 操作対象の行列（※中身が書き換わります）
        nullptr,                    // (任意) デルタ行列を受け取りたい場合
        nullptr,                    // (任意) スナップ値（グリッド吸着など）を指定したい場合
        nullptr,                    // (任意) ローカル境界ボックス
        nullptr                     // (任意) 境界ボックス用のスナップ
    )) {

        ImGuizmo::DecomposeMatrixToComponents(
            worldMat,
            &selectedObj->GetTransform().translate.x,
            &selectedObj->GetTransform().rotate.x, // ラジアン（または度、ライブラリの仕様に合わせて調整）
            &selectedObj->GetTransform().scale.x
        );


        //if (targetObject.worldTransform_.parent_) {
        //    // 親の逆行列を掛けることで、親から見たローカルな行列に変換する
        //    Matrix4x4 invParent = Inverse(targetObject.worldTransform_.parent_->matWorld_);
        //    targetObject.worldTransform_.matWorld_ = Multiply(targetObject.worldTransform_.matWorld_, invParent);

        //    // ローカル行列から再度SRTを抽出し直す
        //    float localMatrix[16];
        //    ConvertMatArray(targetObject.worldTransform_.matWorld_, localMatrix);
        //    
        //    ImGuizmo::DecomposeMatrixToComponents(
        //        localMatrix,
        //        &targetObject.worldTransform_.scale_.x, &targetObject.worldTransform_.rotate_.x, &targetObject.worldTransform_.translate_.x
        //    );
        //}
    };

    //リリース時
    if (!isUsingNow && isUsingPrev) {

        EulerTransform afterTransform;
        afterTransform = selectedObj->GetTransform();

        if (beforeTransform != afterTransform)
        {
            // コマンドマネージャーに登録
            objectCommandManager_.Do<UpdateGuizmoCommand>(
                selectedObj->GetObjectID(),
                beforeTransform,
                afterTransform
            );
        }

    }

    // 状態を次のフレームへ引き継ぐ
    isUsingPrev = isUsingNow;


#endif
}
