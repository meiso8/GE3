#include "ObjectManager.h"
#include"DebugUI.h"
#include"RenderTexture/RenderTexture.h"
#include"SceneManager.h"
#include"Model.h"

#ifdef USE_IMGUI
#include"ImGuizmo.h"
#endif 

#include"Input.h"
#include"JsonFile.h"

#include"Log.h"
#include"CollisionManager.h"
#include"CollisionConfig.h"
#include"MakeMatrix.h"

namespace MatArray {

    void ConvertMatArray(const Matrix4x4& srcMatrix, float dstArray[16])
    {
        // 行列のメモリ構造をそのまま16個のfloat配列にコピー
        std::memcpy(dstArray, &srcMatrix.m[0][0], sizeof(float) * 16);
    }
    void ConvertArrayToMat(const float dstArray[16], Matrix4x4& srcMatrix) {
        std::memcpy(&srcMatrix.m[0][0], dstArray, sizeof(float) * 16);

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

    //colliderMapを削除する
// コライダーマップから削除する際、コライダー内の object3d_ も解除しておく
    auto itr = colliderMaps_.find(gameObject);
    if (itr != colliderMaps_.end()) {
        colliderMaps_.erase(itr);
    }
    // 【追加】実体を管理している unique_ptr の配列からも削除して完全に解放する
    auto itCreate = std::remove_if(createObjects_.begin(), createObjects_.end(),
        [gameObject](const std::unique_ptr<Object3d>& obj) {
            return obj.get() == gameObject;
        });
    if (itCreate != createObjects_.end()) {
        createObjects_.erase(itCreate, createObjects_.end());
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

    //セーブフラグ
    bool isSave = false;
    if (ImGui::Button("Save")) {
        isSave = true;
    }
    //同じラインに表示
    ImGui::SameLine();
    DebugUI::FindJsonFile(jsonFileName_, true, kJsonFileName_.c_str());

    //オブジェクトの生成
    if (ImGui::Button("CreateObject")) {
        CreateObject();
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

 
    ImGui::Checkbox("edit", &editCollider_);

    if (clickedID_ != 0) {
        isClicked = SelectObject(camera);
    }

    auto* selectedObj = ObjectManager::GetInstance()->FindObjectByID(clickedID_);

    if (selectedObj) {
        DebugUI::CheckObject3d(*selectedObj);
        colliderObjectPtr_ = selectedObj;
    }

    ColliderCheck(camera);

    if (!isClicked && !ImGui::GetIO().WantCaptureMouse) {
        //ImGuiでどこにもマウスがキャプチャーしてないとき
        clickedID_ = renderTexture_->GetClickedObjectID();
    }

    if (isSave) {
        Save();
    }

    ImGui::End();

#endif
}

void ObjectManager::SetRenderTextureForModel(RenderTexture* rendterTexture)
{
    renderTexture_ = rendterTexture;
}

void ObjectManager::Clear() {
    objects_.clear();
    idMap_.clear();
    createObjects_.clear();
    colliderMaps_.clear();
    nextID_ = 1; // IDのリセット
    clickedID_ = 0;
    editCollider_ = false;
    colliderObjectPtr_ = nullptr;
}

void ObjectManager::Initialize()
{
    objectCommandManager_.Initialize();
    editCollider_ = false;
    colliderObjectPtr_ = nullptr;
    LogFile::Log("ObjectManager Initialize");
}

void ObjectManager::SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    Object3d::SetCommandListAndSrvDescriptorHeap(commandList, srvDescriptorHeap);
    LogFile::Log("Objects Set CommandList And Srv DescriptorHeap\n");
}

void ObjectManager::Update()
{
    for (auto& obj : createObjects_) {
        obj->Update();
    }
}

void ObjectManager::CheckCollision(CollisionManager* collisionManager)
{
    for (auto& [objPtr, collider] : colliderMaps_) {
        collisionManager->AddCollider(collider.get());
    }
}

void ObjectManager::Draw(Camera& camera)
{
    for (auto& obj : createObjects_) {
        obj->Draw(camera);
    }

#ifdef _DEVELOP

    for (auto& [objPtr, collider] : colliderMaps_) {
        collider->ColliderDraw(camera);
    }

#endif
}

void ObjectManager::Finalize()
{
    createObjects_.clear();
    colliderMaps_.clear();
}

void ObjectManager::Save()
{

    nlohmann::json& json = JsonFile::GetJsonFiles(jsonFileName_);
    std::string sceneName = SceneManager::GetCurrentSceneName() + "Scene";
    //シーン名
    json["name"] = sceneName;

    // 一度配列をクリアする
    json["objects"] = nlohmann::json::array();

    for (auto& object : objects_) {

        // オブジェクト名の取得
        std::string name = object->GetObjectName() + std::to_string(object->GetObjectID());

        std::string meshName = "empty";

        auto* primitive = object->GetPrimitive();
        if (primitive) {
            meshName = primitive->GetMeshName();
        }

        nlohmann::json objectJson = {
            {"file_name", meshName},
            {"transform", JsonFile::EulerTransformToJson(object->GetTransform())},
            {"disabled", object->GetDisabled()},
            {"temperature",object->GetTemperature()},
            {"color",JsonFile::Vector4ToJson(object->GetColor())},
            {"shininess",object->GetShininess()},
            {"environmentCoefficient",object->GetEnvironmentCoefficient()},
            {"name", name},
            {"type", object->GetObjectType() },
            {"nextStageName",object->GetNextStageName()},

        };

        //輝度を追加

        //モデルだった場合ディレクトリパスの要素を追加
        if (auto model = dynamic_cast<Model*>(primitive)) {
            objectJson["directoryPath"] = model->GetModelData()->directoryPath_;
        } else {
            //テクスチャハンドルを設定する
            objectJson["textureHandle"] = object->GetTextureHandle();
        }

        if (colliderMaps_.contains(object)) {

            auto& collider = colliderMaps_.at(object);

            Vector3 size = { 0.0f };
            std::string typeName = "";
            if (collider->GetType() == Collider::ColliderType::kAABB) {
                AABB aabb = collider->GetAABB();
                size = aabb.max - aabb.min;
                typeName = "AABB";
            }
            if (collider->GetType() == Collider::ColliderType::kSphere) {
                float sizeX = collider->GetRadius() * 2.0f;
                size = { sizeX ,sizeX ,sizeX };
                typeName = "Sphere";
            }

            objectJson["collider"] = {
                {"tag",CollisionTag::GetTagName(collider->GetCollisionAttribute())},
                {"maskTag", collider->GetCollisionMask()},
                {"type",typeName},
                {"center",JsonFile::Vector3ToJson(collider->GetCenter())},
                { "size", JsonFile::Vector3ToJson(size)},
            };
        }

        // 4. 配列に要素を追加
        json["objects"].push_back(objectJson);
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

void ObjectManager::CreateObject()
{
    std::unique_ptr<Object3d> newObject = std::make_unique<Object3d>();
    newObject->Create();
    newObject->SetMeshAndMaterial(ModelManager::GetModel("medjed.gltf"));
    //メッシュ
    newObject->SetObjectType("MESH");
    RegisterObject(newObject.get());

    createObjects_.push_back(std::move(newObject));
}

bool ObjectManager::SelectObject(Camera& camera)
{
    Object3d* selectedObj = FindObjectByID(clickedID_);
    return UpdateImGuizmo(camera, selectedObj);
}

bool ObjectManager::UpdateImGuizmo(Camera& camera, Object3d* object3d)
{
    if (editCollider_|| object3d == nullptr) {
        return false;
    }
#ifdef USE_IMGUI
    // 1. ImGuizmoのフレーム開始宣言（内部のコンテキストを初期化・更新します）
    ImGuizmo::BeginFrame();

    static ImGuizmo::MODE currentMode = ImGuizmo::LOCAL;
    static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;// TRANSLATE, ROTATE, SCALE
    static bool snap = false;
    SelectMovement(currentMode, currentOperation, snap);

    ImGuiIO& io = ImGui::GetIO();
    //画面全域に合わせる
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    static EulerTransform beforeTransform;

    //前フレームでギズモ使用しているか
    static bool isUsingPrev = false;
    bool isUsingNow = ImGuizmo::IsUsing();

    //  ドラッグが始まった瞬間トリガーの状態を保存
    if (isUsingNow && !isUsingPrev) {
        beforeTransform = object3d->GetTransform();
    }

    float viewMat[16] = { 0.0f };
    float projectionMat[16] = { 0.0f };
    float worldMat[16] = { 0.0f };

    MatArray::ConvertMatArray(camera.GetViewMatrix(), viewMat);
    MatArray::ConvertMatArray(camera.GetProjectionMatrix(), projectionMat);
    MatArray::ConvertMatArray(object3d->GetWorldMatrix(), worldMat);


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

        EulerTransform& transform = object3d->GetTransform();
        transform.translate = { targetTranslation[0], targetTranslation[1], targetTranslation[2] };
        transform.scale = { targetScale[0], targetScale[1], targetScale[2] };

        //DegreeからラジアンRadianへ変換して代入
        constexpr float ToRadian = std::numbers::pi_v<float> / 180.0f;
        transform.rotate = {
            targetRotation[0] * ToRadian,
            targetRotation[1] * ToRadian,
            targetRotation[2] * ToRadian
        };

    };

    //リリース時
    if (!isUsingNow && isUsingPrev) {

        EulerTransform afterTransform = object3d->GetTransform();

        if (beforeTransform != afterTransform)
        {
            // コマンドマネージャーに登録
            objectCommandManager_.Do<UpdateGuizmoCommand>(
                object3d->GetObjectID(),
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

bool ObjectManager::UpdateImGuizmoForCollider(Camera& camera, Collider* collider, const Matrix4x4& objectMat)
{
    if (!editCollider_) {
        return false;
    }

#ifdef USE_IMGUI
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);
    static ImGuizmo::MODE currentMode = ImGuizmo::LOCAL;
    static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;// TRANSLATE, ROTATE, SCALE
    static bool  useSnap = false;
    SelectMovement(currentMode, currentOperation, useSnap);

    ImGuiIO& io = ImGui::GetIO();
    //画面全域に合わせる
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    float viewMat[16] = { 0.0f };
    float projectionMat[16] = { 0.0f };
    float worldMat[16] = { 0.0f };

    MatArray::ConvertMatArray(camera.GetViewMatrix(), viewMat);
    MatArray::ConvertMatArray(camera.GetProjectionMatrix(), projectionMat);

    Vector3 center = collider->GetCenter();
    Vector3 scale = { 0.0f };

    if (collider->GetType() == Collider::ColliderType::kAABB) {
        scale = Math::AABBSize(collider->GetAABB());

    } else if (collider->GetType() == Collider::ColliderType::kSphere) {
        float diameter = collider->GetRadius() * 2.0f;
        scale = { diameter ,diameter ,diameter };
    }

    Matrix4x4 child = MakeAffineMatrix(scale, Vector3{ 0.0f,0.0f,0.0f }, center);
    MatArray::ConvertMatArray(child * objectMat, worldMat);

    static float snapP = 1.0f;

    if (useSnap) {
        ImGui::SliderFloat("snap", &snapP, 0.0f, 10.0f);
    }

    float snapValues[3] = { snapP, snapP, snapP };

    bool isManipulated = ImGuizmo::Manipulate(
        viewMat,          // float[16]
        projectionMat,    // float[16]
        currentOperation, // 現在の操作（移動・回転・拡大縮小）
        currentMode,      // 座標系（WORLD / LOCAL）
        worldMat,         // 操作対象の行列
        nullptr,
        useSnap ? snapValues : nullptr // スナップ指定
    );

    if (isManipulated) {
        Matrix4x4 newColliderWorldMat;
        MatArray::ConvertArrayToMat(worldMat, newColliderWorldMat); // ※配列からMatrixに戻す関数があると便利です

        // 3. 親の逆行列（Inverse）を掛けることで、新しい「ローカル行列（newChild）」を取り出す
        Matrix4x4 invObjectMat = Inverse(objectMat);
        Matrix4x4 newChild = newColliderWorldMat * invObjectMat;

        Vector3 newCenter = {
            newChild.m[3][0],
            newChild.m[3][1],
            newChild.m[3][2]
        };

        collider->SetCenter(newCenter);

        // 各軸の長さ（ベクトルの大きさ）からスケールを計算
        Vector3 newScale = {
            Length({ newChild.m[0][0], newChild.m[0][1], newChild.m[0][2] }),
            Length({ newChild.m[1][0], newChild.m[1][1], newChild.m[1][2] }),
            Length({ newChild.m[2][0], newChild.m[2][1], newChild.m[2][2] })
        };

        if (collider->GetType() == Collider::ColliderType::kAABB) {
            float halfX = fabsf(newScale.x) * 0.5f;
            float halfY = fabsf(newScale.y) * 0.5f;
            float halfZ = fabsf(newScale.z) * 0.5f;

            collider->SetAABB({
                .min = { -halfX, -halfY, -halfZ },
                .max = {  halfX,  halfY,  halfZ }
                });
        } else if (collider->GetType() == Collider::ColliderType::kSphere) {
            float radius = fabsf(newScale.x) * 0.5f;
            collider->SetRadius(radius);

        }

        return true;
    };
#endif

    return false;
}

void ObjectManager::SelectMovement(ImGuizmo::MODE& currentMode, ImGuizmo::OPERATION& currentOperation, bool& snap)
{

#ifdef USE_IMGUI
    static int mode = ImGuizmo::LOCAL;

    if (ImGui::RadioButton("Local", &mode, 0)) {
        currentMode = ImGuizmo::LOCAL;
        mode = ImGuizmo::LOCAL;
    }
    ImGui::SameLine(); // 横並びにする場合
    if (ImGui::RadioButton("World", &mode, 1)) {
        currentMode = ImGuizmo::WORLD;
        mode = ImGuizmo::WORLD;
    }
    ImGui::SameLine(); // 横並びにする場合
    ImGui::Checkbox("Snap", &snap);

    static int currentOp = 0; // 0: TRANSLATE, 1: ROTATE, 2: SCALE

    if (ImGui::RadioButton("Grab : G", &currentOp, 0) || ImGui::IsKeyPressed(ImGuiKey_G)) {
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
#endif

}

void ObjectManager::ColliderCheck(Camera& camera)
{

    if (!editCollider_ || colliderObjectPtr_ == nullptr) {
        return;
    }
#ifdef _DEVELOP
    if (ImGui::TreeNode("Collider")) {

        if (!colliderMaps_.contains(colliderObjectPtr_)) {
            if (ImGui::Button("AddCollider")) {
                colliderMaps_[colliderObjectPtr_] = std::make_unique<Collider>();
                //コライダーに自分のワールドマトリックスを追加する
                colliderMaps_[colliderObjectPtr_]->SetWorldMatrix(colliderObjectPtr_->GetWorldTransform().matWorld_);
            }
        }

        if (editCollider_) {
            if (colliderMaps_.contains(colliderObjectPtr_)) {
                auto& collider = colliderMaps_.at(colliderObjectPtr_);
                DebugUI::CheckCollider(*collider, "collider");
                UpdateImGuizmoForCollider(camera, collider.get(), colliderObjectPtr_->GetWorldMatrix());
            }
        }

        ImGui::TreePop();

    }
#endif

}
