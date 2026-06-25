#include "ObjectManager.h"
#include"DebugUI.h"
#include"RenderTexture/RenderTexture.h"

#ifdef USE_IMGUI
#include"ImGuizmo.h"
#endif 

#include"Input.h"
#include"JsonFile.h"

#include"AnimationObject3d.h"
#include"BeamObject3d.h"
#include"SkyBoxObject3d.h"
#include"LineObject3d.h"

#include"Log.h"

namespace {

    void ConvertMatArray(const Matrix4x4& srcMatrix, float dstArray[16])
    {
        // 行列のメモリ構造をそのまま16個のfloat配列にコピー
        std::memcpy(dstArray, &srcMatrix.m[0][0], sizeof(float) * 16);
    }
}

ObjectManager* ObjectManager::GetInstance() {
    static ObjectManager instance;
    return &instance;
}

void ObjectManager::RegisterObject(Object3d* gameObject) {
    if (!gameObject) return;

    uint32_t allocatedID = gameObject->GetObjectID();

    if (allocatedID == 0) {
        // 新しいIDを割り当てる（Allocate）
        allocatedID = nextID_++;
    }

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
    // 見つからない（背景など）
    return nullptr;
}

void ObjectManager::ClickObject(Camera& camera)
{
#ifdef USE_IMGUI

    ImGui::Begin("Object3ds");

    DebugUI::CreateJsonFile(kJsonFileName_.c_str());
    //フィルターかけてObjecFileNameを探す
    DebugUI::FindJsonFile(jsonFileName_,true,kJsonFileName_.c_str());
    //同じラインに表示
    ImGui::SameLine();
    //セーブフラグ
    bool isSave = false;
    if (ImGui::Button("Save")) {
        isSave = true;
    }

    // 保存完了メッセージを表示
    DebugUI::ShowJsonFileSaveMessage(jsonFileName_);

    bool isClicked = false;

    ImGui::Text("ClickedID : %d", clickedID_);

    bool isTriggerCtrlZ = Input::IsPressKey(DIK_LCONTROL) && Input::IsTriggerKey(DIK_Z);

    bool doRedo = (Input::IsPressKey(DIK_LSHIFT) && isTriggerCtrlZ);
    bool doUndo = isTriggerCtrlZ && !doRedo;

    // 2. Undoボタンの描画 ＆ クリック判定
    if (ImGui::Button("Undo")) {
        doUndo = true;
    }
    ImGui::SameLine();

    // 3. Redoボタンの描画 ＆ クリック判定
    if (ImGui::Button("Redo")) {
        doRedo = true;
    }

    if (doUndo) {
        objectCommandManager_.UnDo();
    } else if (doRedo) {
        objectCommandManager_.ReDo();
    }

    if (clickedID_ != 0) {
        isClicked = UpdateImGuizmo(camera);
    }

    auto* selectedObj = ObjectManager::GetInstance()->FindObjectByID(clickedID_);
    if (selectedObj) {
        DebugUI::CheckObject3d(*selectedObj, "selectObject");
    }

    if (!isClicked && !ImGui::GetIO().WantCaptureMouse) {
        //ImGuiでどこにもマウスがキャプチャーしてないとき
        clickedID_ = RenderTexture::GetInstance()->GetClickedObjectID();
    }

    if (isSave) {
        Save();
    }

    ImGui::End();

#endif
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

void ObjectManager::SeetCommandList(ID3D12GraphicsCommandList* commandList)
{
    Object3d::SetCommandList(commandList);
    AnimationObject3d::SetCommandList(commandList);
    BeamObject3d::SetCommandList(commandList);
    SkyboxObject3d::SetCommandList(commandList);
    LineObject3d::SetCommandList(commandList);
    LogFile::Log("Objects Set CommandList\n");
}

void ObjectManager::Save()
{

    nlohmann::json& json = JsonFile::GetJsonFiles(jsonFileName_);

    for (auto& object : objects_) {
        std::string name = "Object" + std::to_string(object->GetObjectID());

        std::string meshName = "empty";

        if (object->GetPrimitive()) {
            //プリミティブ情報があれば
            meshName = object->GetPrimitive()->GetMeshName();
        }

        json[name] = {
            {"mesh",meshName},
            {"transform", JsonFile::EulerTransformToJson(object->GetTransform())},
           
        };
    }

    // ファイル保存
    JsonFile::SaveJson(jsonFileName_);
    JsonFile::MarkModified(jsonFileName_);

}

void ObjectManager::SetName()
{
    std::string folder = "Resource/JsonFiles/";
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        std::string filename = entry.path().stem().string(); // 拡張子なしのファイル名
        if (filename.find(kJsonFileName_) != std::string::npos) {
            //ObjectEditorが含まれていたらファイルネームをセットする
            jsonFileName_ = filename;
        }
    }
}

bool ObjectManager::UpdateImGuizmo(Camera& camera)
{

    auto* selectedObj = FindObjectByID(clickedID_);

    if (selectedObj == nullptr) {
        return false;
    }
#ifdef USE_IMGUI
    // 1. ImGuizmoのフレーム開始宣言（内部のコンテキストを初期化・更新します）
    ImGuizmo::BeginFrame();

    static ImGuizmo::MODE currentMode = ImGuizmo::LOCAL;
    static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;// TRANSLATE, ROTATE, SCALE

    static int currentOp = 0; // 0: TRANSLATE, 1: ROTATE, 2: SCALE

    if (ImGui::RadioButton("Translate : T", &currentOp, 0) || ImGui::IsKeyPressed(ImGuiKey_T)) {
        currentOperation = ImGuizmo::TRANSLATE;
        currentOp = 0;
    }
    ImGui::SameLine(); // 横並びにする場合

    if (ImGui::RadioButton("Rotate : R", &currentOp, 1) || ImGui::IsKeyPressed(ImGuiKey_R)) {
        currentOperation = ImGuizmo::ROTATE;
        currentOp = 1;
    }
    ImGui::SameLine();

    if (ImGui::RadioButton("Scale : S", &currentOp, 2) || ImGui::IsKeyPressed(ImGuiKey_S)) {
        currentOperation = ImGuizmo::SCALE;
        currentOp = 2;
    }


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

        // 一度ローカルの配列で安全に受け取る
        float targetTranslation[3] = { 0.0f };
        float targetRotation[3] = { 0.0f }; // ※これは「度（Degree）」で返ってくる
        float targetScale[3] = { 0.0f };

        ImGuizmo::DecomposeMatrixToComponents(
            worldMat,
            targetTranslation,
            targetRotation,
            targetScale
        );

        // オブジェクトのTransformへの書き戻し
        // (注: GetTransform() が値をコピーではなく「参照(&)」を返す関数であることを前提としています)
        auto& transform = selectedObj->GetTransform();

        transform.translate = { targetTranslation[0], targetTranslation[1], targetTranslation[2] };
        transform.scale = { targetScale[0], targetScale[1], targetScale[2] };

        //DegreeからラジアンRadianへ変換して代入
        constexpr float ToRadian = std::numbers::pi_v<float> / 180.0f;
        transform.rotate = {
            targetRotation[0] * ToRadian,
            targetRotation[1] * ToRadian,
            targetRotation[2] * ToRadian
        };

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

    return isUsingPrev;

#endif
}
