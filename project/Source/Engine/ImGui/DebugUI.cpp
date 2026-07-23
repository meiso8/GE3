#define NOMINMAX
#include "DebugUI.h"
#include"CharacterState.h"
#include"CbvSrvUavDescriptorHeap.h"
#include"PrimitiveFactory/PrimitiveFactory.h"

#include"Input.h"
#include"Sprite.h"
#include"Primitive.h"
#include"Model.h"
#include"Particle/Particle.h"
#include"Particle/ParticleEmitter.h"
#include"Object3d.h"
#include"AnimationObject3d.h"
#include"ObjectManager/ObjectManager.h"
#include"LevelEditor/LevelEditor.h"
#include"../../Application/GameObject/StageChangeTrigger/StageChangeTrigger.h"
#include"../../Application/StageManager/StageManager.h"
#include"Sound.h"

#include"Lights/Light.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"Lights/PointLightManager.h"
#include"Engine/Mesh/Font/Font.h"

#include"PSO.h"
#include"Camera.h"
#include"JsonFile.h"
#include"TimeManager.h"

#include"SphericalCoordinate.h"

#include"../StageManager/StageManager.h"

#include<numbers>
#include<algorithm>



namespace {
    struct Param {
        char name[128];
        char value[128];
    };

    bool useMaxGameWindow_ = false;
}

void DebugUI::CheckInt(int& value, const char* label) {

#ifdef USE_IMGUI

    if (ImGui::TreeNode(label)) {
        ImGui::SliderInt(label, &value, -100, 100);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckFloat(float& value, const char* label) {

#ifdef USE_IMGUI

    if (ImGui::TreeNode(label)) {
        ImGui::SliderFloat(label, &value, -100.0f, 100.0f);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckCamera(CameraMatrix& cameraMatrix, CameraData& cameraData, const char* label) {

#ifdef USE_IMGUI
    ImGui::Begin("Camera");

    if (ImGui::TreeNode(label)) {

        CheckTransform(cameraData.eTransform, " worldMatrix");
        ShowMatrix4x4(cameraMatrix.worldMat);

        CheckShericalCoordinate(cameraData.sphericalCoordinate);

        ImGui::SliderFloat2("ofsset", &cameraData.offset.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("nearZ", &cameraData.nearZ, 0.0f, 1000.0f);
        ImGui::SliderFloat("farZ", &cameraData.farZ, 0.0f, 1000.0f);
        ImGui::SliderFloat("fovAngleY", &cameraData.fovAngleY, -6.28f, 6.28f);
        ImGui::Text("Type : %s", (cameraData.projectionType == PERSPECTIVE) ? "PERSPECTIVE" : "PARALLEL");

        if (ImGui::Button("ChangeType")) {
            cameraData.projectionType = (cameraData.projectionType == PERSPECTIVE) ? PARALLEL : PERSPECTIVE;
        }

        ImGui::TreePop();

    }

    ImGui::End();
#endif
}

void DebugUI::CheckShericalCoordinate(SphericalCoordinate& sphericalCoordinate)
{

#ifdef USE_IMGUI

    if (ImGui::TreeNode("ShericalCoordinate")) {
        ImGui::SliderFloat("polar", &sphericalCoordinate.polar, -10.0f, 10.0f);
        ImGui::SliderFloat("azimuthal", &sphericalCoordinate.azimuthal, -10.0f, 10.0f);
        ImGui::SliderFloat("radius", &sphericalCoordinate.radius, -100.0f, 100.0f);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckEmitter(Emitter& emitter, const char* label)
{

#ifdef USE_IMGUI
    ImGui::Begin("Particle");

    if (ImGui::TreeNode(label)) {

        int movement = static_cast<int>(emitter.movement);
        ImGui::SliderInt("movement", &movement, 0, 3);
        emitter.movement = static_cast<ParticleMovements>(movement);
        ImGui::Separator();
        int count = emitter.count;
        ImGui::SliderInt("createNum", &count, 0, ParticleManager::GetMaxInstance());
        emitter.count = count;
        ImGui::Checkbox("isLoop", &emitter.isLoop_);
        ImGui::Text("frequencyTime : %f", emitter.frequencyTime);
        ImGui::SliderFloat("frequency", &emitter.frequency, 0.001f, 10.0f);
        ImGui::SliderFloat("lifeTime", &emitter.lifeTime, -1.0f, 50.0f);

        ImGui::Checkbox("useBillboard", &emitter.useBillboard_);

        if (ImGui::TreeNode("Acceleration")) {
            ImGui::SliderFloat3("acceleration", &emitter.accelerationField_.acceleration.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("area.min", &emitter.accelerationField_.area.min.x, -100.0f, 0.0f);
            ImGui::SliderFloat3("area.max", &emitter.accelerationField_.area.max.x, 0.0f, 100.0f);
            ImGui::TreePop();
        }

        ImGui::Checkbox("useRadialEmission", &emitter.useRadialEmission_);
        ImGui::Separator();
        CheckWorldTransform(emitter.transform, "transform");

        ImGui::Separator();
        if (ImGui::TreeNode("transformMinMax")) {
            ImGui::SliderFloat3("scaleAABBMin", &emitter.scaleAABB_.min.x, -20.0f, 0.0f);
            ImGui::SliderFloat3("scaleAABBMax", &emitter.scaleAABB_.max.x, 0.0f, 20.0f);
            ImGui::Separator();
            ImGui::SliderFloat3("rotateAABBMin", &emitter.rotateAABB_.min.x, -20.0f, 0.0f);
            ImGui::SliderFloat3("rotateAABBMax", &emitter.rotateAABB_.max.x, 0.0f, 20.0f);
            ImGui::Separator();
            ImGui::SliderFloat3("translateMin", &emitter.translateAABB_.min.x, -20.0f, 0.0f);
            ImGui::SliderFloat3("translateMax", &emitter.translateAABB_.max.x, 0.0f, 20.0f);
            ImGui::TreePop();
        }

        ImGui::Separator();
        ImGui::SliderFloat3("velcityAABBMax", &emitter.velocityAABB.min.x, -20.0f, 0.0f);
        ImGui::SliderFloat3("velcityAABBMin", &emitter.velocityAABB.max.x, 0.0f, 20.0f);
        ImGui::Separator();

        ImGui::SliderFloat("radius", &emitter.radius, 0.1f, 10.0f);
        ImGui::SliderFloat("radiusSpeed", &emitter.radiusSpeed, -100.f, 100.0f);

        ImGui::Separator();
        ImGui::SliderFloat("polarSpeed", &emitter.polarSpeed, -100.f, 100.0f);
        ImGui::SliderFloat("polarSpeedAABBMin", &emitter.polarSpeedMinMax.min, -20.0f, 0.0f);
        ImGui::SliderFloat("polarSpeedAABBMax", &emitter.polarSpeedMinMax.max, 0.0f, 20.0f);
        ImGui::Separator();

        CheckBlendMode(emitter.blendMode);
        CheckColor(emitter.startColor, "startColor");
        CheckColor(emitter.endColor, "endColor");

        ImGui::TreePop();
    }

    ImGui::End();

#endif
}


void DebugUI::CheckJsonFile()
{
#ifdef USE_IMGUI

    ImGui::Begin("Json");

        //新しくJsonFileを作成する
        CreateJsonFile();

    if (ImGui::TreeNode("FindTag")) {

        std::string tagName = "";

        nlohmann::json& jsonfile = FindJsonFile(tagName);

        ImGui::Separator();

        if (ImGui::TreeNode("ShowJsonData")) {
            ImGui::Text("Name: %s", tagName.c_str());
            ImGui::TextWrapped("Data: %s", jsonfile.dump(2).c_str());
            ImGui::TreePop();
            ImGui::Separator();
        }

        static char structName[128] = "structName";
        static std::vector<Param> params = {};

        if (ImGui::TreeNode("AddParam")) {

            if (ImGui::InputText("StructName", structName, IM_ARRAYSIZE(structName))) {
                JsonFile::ClearModified(tagName);
            }

            for (size_t i = 0; i < params.size(); ++i) {
                if (ImGui::TreeNode(("param" + std::to_string(i)).c_str())) {

                    if (ImGui::InputText(("Name##" + std::to_string(i)).c_str(), params[i].name, IM_ARRAYSIZE(params[i].name))) {
                        JsonFile::ClearModified(tagName);
                    }

                    if (ImGui::InputText("Value##", params[i].value, IM_ARRAYSIZE(params[i].value))) {
                        JsonFile::ClearModified(tagName);
                    }

                    // 削除ボタン
                    if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                        params.erase(params.begin() + i);
                        ImGui::TreePop(); // 消したあとに TreeNode を閉じておく
                        break; // erase したらループを抜ける（インデックスがズレるのを防ぐため）
                    }

                    ImGui::TreePop();
                }
            }

            // パラメータ追加ボタン
            if (ImGui::Button("Add New Param")) {
                params.push_back({ "newParam", "" });
                JsonFile::ClearModified(tagName);
            }


            ImGui::TreePop();
        }

        if (ImGui::Button("Save")) {

            for (const auto& param : params) {
                jsonfile[structName][param.name] = param.value;
            }
            // ファイル保存
            JsonFile::SaveJson(tagName);
            JsonFile::MarkModified(tagName);

        }

        // 保存完了メッセージを表示
        ShowJsonFileSaveMessage(tagName);


        ImGui::TreePop();
    }

    ImGui::End();
#endif
}

void DebugUI::ShowJsonFileSaveMessage(const std::string& name) {

#ifdef USE_IMGUI

    // 保存完了メッセージを表示
    if (JsonFile::IsModified(name)) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "File saved");
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "File not saved.");
    }
#endif
}


nlohmann::json& DebugUI::FindJsonFile(std::string& tagName, bool useFilter, const char* containFileName)
{

#ifdef USE_IMGUI

    if (ImGui::TreeNode("FindJsonFiles")) {

        // 安定した文字列保持用
        static std::vector<std::string> tagStrings;
        static std::vector<const char*> tagOptions;

        tagStrings.clear();
        tagOptions.clear();

        for (const auto& [tag, data] : JsonFile::GetJsonData()) {

            if (!useFilter || tag.find(containFileName) != std::string::npos) {
                tagStrings.push_back(tag); // 条件に合う std::string のみを保持
            }
        }

        for (const auto& str : tagStrings) {
            tagOptions.push_back(str.c_str()); // 安定したポインタを取得
        }

        // ImGui::Combo に渡す
        static int tag_current = 0;

        // 選択肢が空になった場合の安全対策
        if (tagOptions.empty()) {
            ImGui::Text("No matching tags found.");
            tagName = "";
            // 例外処理
            static nlohmann::json empty_json;
            return empty_json;
        }

        if (ImGui::Combo(containFileName, &tag_current, tagOptions.data(), static_cast<int>(tagOptions.size()))) {
            ImGui::Text("Tag: %s", tagOptions[tag_current]);
        }

        ImGui::TreePop();

        if (tag_current >= tagOptions.size()) {

            static nlohmann::json empty_json;
            tagName = "";
            return empty_json;
        }

        tagName = tagOptions[tag_current];
        return JsonFile::GetJsonFiles(tagName);

    }
#endif
    static nlohmann::json empty_json;
    tagName = "";
    return empty_json;
}

void DebugUI::CreateJsonFile(const char* containFileName)
{
#ifdef USE_IMGUI

    static char tagBuffer[128] = "";
    static bool isInitialized = false;
    bool isCreate = false;

    // 初回呼び出し時のみ、引数で渡された文字列をバッファにコピー
    if (!isInitialized && containFileName != nullptr) {
        // 安全のためにバッファサイズを超えないようにコピー
        strncpy(tagBuffer, containFileName, sizeof(tagBuffer) - 1);
        tagBuffer[sizeof(tagBuffer) - 1] = '\0'; // 終端ヌル文字を保証
        isInitialized = true;
    }

    if (ImGui::Button("Create")) { isCreate = true; }
    //同じラインに表示
    ImGui::SameLine();
    //ファイルタグ名を入力
    ImGui::InputText("FileTag", tagBuffer, IM_ARRAYSIZE(tagBuffer));

    if (isCreate) {
        // 新しい構造化JSONを作成
        nlohmann::json newJson;
        // 管理マップに登録
        JsonFile::SetJson(tagBuffer, newJson);
        JsonFile::MarkModified(tagBuffer);
    }

#endif
}

void DebugUI::CheckSRVIndex(CbvSrvUavDescriptorHeap* srvDescriptorHeap) {
#ifdef USE_IMGUI
    static int index = 0;

    ImGui::Begin("SRVTexture");
    // 例：表示したいSRVのインデックス番号
    // （テクスチャを読み込んだ時のインデックスや、RenderTextureのsrvIndexなど）
    ImGui::SliderInt("srvIndex", &index, 0, CbvSrvUavDescriptorHeap::kMaxCount_ - 1);

    // SrvManager から GPUハンドルを取得
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvDescriptorHeap->GetGPUDescriptorHandle(index);

    // ImGui::Imageに渡すために ImTextureID (void* 型) にキャストする
    ImTextureID texID = (ImTextureID)gpuHandle.ptr;

    // 画像の表示 (引数: テクスチャID, 表示サイズ(横, 縦))
    ImGui::Image(texID, ImVec2(128.0f, 72.0f));


    ImGui::End();
#endif
}

void DebugUI::CheckSRVTexture(const int srvIndex, CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
#ifdef USE_IMGUI

    if (srvIndex >= (int)CbvSrvUavDescriptorHeap::kMaxCount_) {
        return;
    };

    // SrvManager から GPUハンドルを取得
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvDescriptorHeap->GetGPUDescriptorHandle(srvIndex);

    // ImGui::Imageに渡すために ImTextureID (void* 型) にキャストする
    ImTextureID texID = (ImTextureID)gpuHandle.ptr;

    // 画像の表示 (引数: テクスチャID, 表示サイズ(横, 縦))
    ImGui::Image(texID, ImVec2(128.0f, 72.0f));

#endif
}

void DebugUI::CheckTextures(CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{

#ifdef USE_IMGUI

    if (ImGui::TreeNode("Textures")) {

        const std::vector<uint32_t>& srvIndexes = Texture::GetMappedSRVIndexes();

        // アイテム1個あたりの横幅（Imageの72.0f + 余裕を持たせたパディング）
        const float itemWidth = 72.0f + ImGui::GetStyle().ItemSpacing.x;

        for (int i = 0; i < (int)srvIndexes.size(); ++i) {


            // ★ 折り返し計算
            if (i > 0) {
                // 現在の行の残り横幅を取得
                float lastX = ImGui::GetItemRectMax().x;
                float nextX = lastX + itemWidth;
                float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

                // 次のアイテムを置くスペースが残っている場合だけ横に並べる
                if (nextX < windowVisibleX) {
                    ImGui::SameLine();
                }
                // スペースがなければ SameLine() を呼ばないことで自動的に次の行（改行）になる
            }
            // 1つの項目を縦にまとめるためのグループ化
            ImGui::BeginGroup();

            uint32_t currentSrvIndex = srvIndexes[i];

            // ★ 0 などの未割り当て、あるいは無効な定数(0xFFFFFFFF等)の場合は描画しないガードを入れる
            if (currentSrvIndex != 0 && currentSrvIndex < CbvSrvUavDescriptorHeap::kMaxCount_ && !Texture::GetMetaData(currentSrvIndex).IsCubemap()) {

                // 安全であることを確認してからハンドルを取得
                D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvDescriptorHeap->GetGPUDescriptorHandle(currentSrvIndex);

                if (gpuHandle.ptr != 0) {
                    ImGui::Text("[%d] SRV:%d", i, currentSrvIndex);
                    ImTextureID texID = (ImTextureID)gpuHandle.ptr;
                    ImGui::Image(texID, ImVec2(72.0f, 72.0f));
                }
            } else {
                if (Texture::GetMetaData(currentSrvIndex).IsCubemap()) {
                    ImGui::Text("[%d] Cube", i, currentSrvIndex);

                } else {
                    ImGui::Text("[%d] Skip", i, currentSrvIndex);
                };

                // 代わりに空白（ダミー領域）を作って高さを揃える
                ImGui::Dummy(ImVec2(72.0f, 72.0f));
            }

            ImGui::EndGroup(); // グループ化終了
        }

        ImGui::TreePop();

    }

#endif
}

void DebugUI::CheckStageManager()
{
#ifdef USE_IMGUI
    auto* stageManager = StageManager::GetInstance();
    // 現在トリガーに設定されている遷移先ステージ名を取得
    std::string currentStageName = stageManager->GetCurrentStageName();

    if (ImGui::BeginCombo("StageName", currentStageName.c_str())) {
        // オブジェクト名を選択肢に入れる
        for (auto [stage, nextStageName] : stageManager->GetStageNames()) {
            // 選択肢を表示（クリックされたら true を返す）
            if (ImGui::Selectable(nextStageName.c_str(), true)) {
                // クリックされたらStageNameをセットする
                stageManager->SetNestStage(nextStageName);
                break;
            }
        }

        ImGui::EndCombo();
    }

#endif
}

void DebugUI::CheckSpotLight()
{
#ifdef USE_IMGUI

    if (ImGui::TreeNode("SpotLights")) {

        for (int i = 0; i < SpotLightManager::kMaxData_; ++i) {

            if (ImGui::TreeNode(("light " + std::to_string(i)).c_str())) {
                SpotLight& spotLight = SpotLightManager::GetData(i);
                Vector3& direction = spotLight.direction;

                CheckColor(spotLight.color, "color");
                ImGui::SliderFloat("intensity", &spotLight.intensity, 0.0f, 100.0f);
                ImGui::DragFloat3("position", &spotLight.position.x, 0.03f, -10000.0f, 10000.0f);
                ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f);
                direction = Normalize(direction);
                ImGui::SliderFloat("distance", &spotLight.distance, 0.0f, 100.0f);
                ImGui::SliderFloat("decay", &spotLight.decay, 0.0f, 100.0f);
                ImGui::SliderFloat("cosAngle", &spotLight.cosAngle, -6.28f, 6.28f);
                ImGui::TreePop();

            }



        }
        ImGui::TreePop();
    }


#endif
}

void DebugUI::CheckModels() {

#ifdef USE_IMGUI


    if (ImGui::TreeNode("Models")) {

        for (auto& [name, model] : ModelManager::GetModels()) {

            if (ImGui::TreeNode(name.string().c_str())) {

                auto* modelData = model->GetModelData();

                ImGui::Text("indices : Size : %d", modelData->indices.size());
                ImGui::Text("vertices : Size : %d", modelData->vertices.size());

                if (ImGui::TreeNode("material")) {
                    for (auto& [name, materials] : modelData->materials) {
                        if (ImGui::TreeNode(name.c_str())) {
                            for (int i = 0; i < materials.textureData_.size(); ++i) {
                                ImGui::Text("material : SrvIndex : %d", materials.textureData_[i].textureSrvIndex);
                            }
                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
        };
        ImGui::TreePop();
    }

#endif
}


void DebugUI::CheckInput() {
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Input")) {

        ImGui::SliderFloat2("mousePos", &Input::GetMousePos().x, 0.0f, 1280.0f);
        ImGui::SliderFloat2("cursorPos", &Input::GetCursorPosition().x, 0.0f, 1280.0f);

        for (int i = 0; i < 4; ++i) {
            CheckXInput(i);
        }

        ImGui::TreePop();
    }
#endif

}

void DebugUI::CheckXInput(const int& num)
{
#ifdef USE_IMGUI
    std::string numOK = std::to_string(num);
    if (ImGui::TreeNode(numOK.c_str())) {

        ImGui::Text("Controller %s", Input::GetIsControllerConnected(num) ? "Connected" : "Unkown");
        ImGui::Text("left %d", Input::GetControllerTriggerCount(BUTTON_LEFT, num));
        ImGui::Text("right %d", Input::GetControllerTriggerCount(BUTTON_RIGHT, num));

        ImGui::Text("DPAD_UP %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_DPAD_UP, num));
        ImGui::Text("DPAD_DOWN %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_DPAD_DOWN, num));
        ImGui::Text("DPAD_LEFT %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_DPAD_LEFT, num));
        ImGui::Text("DPAD_RIGHT %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_DPAD_RIGHT, num));
        ImGui::Text("START %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_START, num));
        ImGui::Text("BACK %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_BACK, num));
        ImGui::Text("LEFT_THUMB %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_LEFT_THUMB, num));
        ImGui::Text("RIGHT_THUMB %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_RIGHT_THUMB, num));
        ImGui::Text("LEFT_SHOULDER %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER, num));
        ImGui::Text("RIGHT_SHOULDER %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER, num));
        ImGui::Text("A %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_A, num));
        ImGui::Text("B %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_B, num));
        ImGui::Text("X %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_X, num));
        ImGui::Text("Y %d", Input::IsControllerPressButton(XINPUT_GAMEPAD_Y, num));
        Vector2 L = { 0.0f };
        Vector2 R = { 0.0f };

        ImGui::Text("L %d", Input::IsControllerStickPosMove(BUTTON_LEFT, num, &L));
        ImGui::Text("R %d", Input::IsControllerStickPosMove(BUTTON_RIGHT, num, &R));
        ImGui::SliderFloat2("BUTTON_LEFT", &L.x, -32768.0f, 32768.0f);
        ImGui::SliderFloat2("BUTTON_RIGHT", &R.x, -32768.0f, 32768.0f);

        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckLights()
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Lights")) {
        DebugUI::CheckDirectionalLight();
        DebugUI::CheckSpotLight();
        DebugUI::CheckPointLightData();
        ImGui::TreePop();
    }
#endif

}

void DebugUI::CheckSprite(Sprite& sprite, const char* label) {
#ifdef USE_IMGUI
    ImGui::Begin("Sprite");

    if (ImGui::TreeNode(label)) {

        if (ImGui::TreeNode("transform2D")) {
            ImGui::SliderFloat2("pos", &sprite.GetPosition().x, -1280.0f, 1280.0f);
            ImGui::SliderFloat("rotation", &sprite.GetRotate(), 0.0f, std::numbers::pi_v<float>*2.0f);
            ImGui::SliderFloat2("scale", &sprite.GetScale().x, -1280.0f, 1280.0f);
            ImGui::SliderFloat2("size", &sprite.GetSize().x, -1280.0f, 1280.0f);
            ImGui::TreePop();
        }

        CheckTransforms(sprite.GetUVScale(), sprite.GetUVRotate(), sprite.GetUVTranslate(), "uvTransform");

        if (ImGui::TreeNode("anchorPointTextureSize")) {

            ImGui::SliderFloat2("anchorPoint", &sprite.GetAnchorPoint().x, 0.0f, 1.0f);
            ImGui::Checkbox("isFlipX", &sprite.GetIsFlipX());
            ImGui::Checkbox("isFlipY", &sprite.GetIsFlipY());
            ImGui::SliderFloat2("textureLeftTop", &sprite.GetTextureLeftTop().x, 0.0f, 1280.0f);
            ImGui::SliderFloat2("textureSize", &sprite.GetTextureSize().x, 0.0f, 1280.0f);
            ImGui::TreePop();
        }

        CheckColor(sprite.GetColor(), "color");

        ImGui::TreePop();
    }



    ImGui::End();
#endif
}

void DebugUI::CheckQuaternionTransform(QuaternionTransform& quaternionTransform, const char* label)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {
        ImGui::SliderFloat3("scale", &quaternionTransform.scale.x, 0.0f, 10.0f);
        ImGui::SliderFloat4("rotation", &quaternionTransform.rotate.x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("translation", &quaternionTransform.translate.x, -1000.0f, 1000.0f);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::ShowMatrix4x4(const Matrix4x4& matrix, const char* label) {
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {
        if (ImGui::BeginTable(label, 4, ImGuiTableFlags_Borders)) {
            for (uint32_t row = 0; row < 4; ++row) {
                ImGui::TableNextRow();
                for (uint32_t col = 0; col < 4; ++col) {
                    ImGui::TableSetColumnIndex(col);
                    ImGui::Text("%.3f", matrix.m[row][col]);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

#endif
}


void DebugUI::CheckFont(Font& font, const char* label)
{
#ifdef USE_IMGUI

    if (ImGui::TreeNode(label)) {

        if (ImGui::TreeNode("transform2D")) {
            ImGui::SliderFloat2("pos", &font.GetPosition().x, -1280.0f, 1280.0f);
            ImGui::SliderFloat("rotation", &font.GetRotate(), 0.0f, std::numbers::pi_v<float>*2.0f);
            ImGui::SliderFloat2("scale", &font.GetScale().x, -1280.0f, 1280.0f);
            ImGui::SliderFloat2("size", &font.GetSize().x, -1280.0f, 1280.0f);
            ImGui::TreePop();
        }

        CheckTransforms(font.GetUVScale(), font.GetUVRotate(), font.GetUVTranslate(), "uvTransform");

        if (ImGui::TreeNode("anchorPointTextureSize")) {

            ImGui::SliderFloat2("anchorPoint", &font.GetAnchorPoint().x, 0.0f, 1.0f);
            ImGui::Checkbox("isFlipX", &font.GetIsFlipX());
            ImGui::Checkbox("isFlipY", &font.GetIsFlipY());
            ImGui::SliderFloat2("textureLeftTop", &font.GetTextureLeftTop().x, 0.0f, 1280.0f);
            ImGui::SliderFloat2("textureSize", &font.GetTextureSize().x, 0.0f, 1280.0f);
            ImGui::TreePop();
        }

        CheckColor(font.GetColor(), "color");

        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckBalloonData(Balloon& balloon)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Balloon")) {
        ImGui::DragFloat("expansionData", &balloon.expansion, 0.03f, 0.0f, 10.0f);
        ImGui::DragFloat("sphere", &balloon.sphere, 0.03f, 0.0f, 1.0f);
        ImGui::DragFloat("cube", &balloon.cube, 0.03f, 0.0f, 1.0f);
        ImGui::Checkbox("isSphere", &balloon.isSphere);
        ImGui::TreePop();
    }
#endif
}
void DebugUI::CheckWaveData(Wave& wave, const char* label)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {

        ImGui::DragFloat("time", &wave.time, 0.03f);
        ImGui::DragFloat("amplitude", &wave.amplitude, 0.03f);
        ImGui::DragFloat3("direction", &wave.direction.x, 0.03f, 0.0f, 1.0f);
        ImGui::SliderFloat("frequency", &wave.frequency, 1.0f, 10.0f);
        Vector3 waveDirection = wave.direction;
        wave.direction = Normalize(waveDirection);
        ImGui::TreePop();
    }
#endif
}
void DebugUI::CheckMeltData(Melt& melt, const char* label)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {

        ImGui::DragFloat("time", &melt.time, 0.03f);
        ImGui::DragFloat("size", &melt.size, 0.03f);
        ImGui::DragFloat("meltTime", &melt.meltTime, 0.03f, 0.0f, 100.0f);
        ImGui::DragFloat("thickness", &melt.thickness, 0.03f, 0.0f, 10.0f);
        ImGui::TreePop();
    }
#endif
}
void DebugUI::CheckPointLightData()
{
#ifdef USE_IMGUI

    if (ImGui::TreeNode("PointLights")) {
        for (int i = 0; i < PointLightManager::kMaxData_; ++i) {

            if (ImGui::TreeNode(("light " + std::to_string(i)).c_str())) {
                PointLight& pointLight = PointLightManager::GetData(i);
                CheckColor(pointLight.color, "color");
                ImGui::SliderFloat("intensity", &pointLight.intensity, 0.0f, 100.0f);
                ImGui::DragFloat3("position", &pointLight.position.x, 0.03f, -10000.0f, 10000.0f);
                ImGui::SliderFloat("radius", &pointLight.radius, 0.0f, 100.0f);
                ImGui::SliderFloat("decay", &pointLight.decay, 0.0f, 100.0f);
                ImGui::TreePop();

            }

        }
        ImGui::TreePop();
    }

#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForRenderTexture& material)
{

#ifdef USE_IMGUI
    if (ImGui::TreeNode("GrayScale")) {
        DebugUI::CheckColor(material.color, "RenderTextureColor");
        ImGui::Checkbox("UseEffect", &material.useEffect);
        const char* lights[] = { "Sepia", "Grayscale", "None" };
        int type_current = int(material.type);

        if (ImGui::Combo("EffectType", &type_current, lights, IM_ARRAYSIZE(lights))) {
            material.type = type_current % 3;
        };
        ImGui::SliderFloat("effectVol", &material.effectVol,0.0f,1.0f);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForVignette& material)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Vignette")) {
        DebugUI::CheckFloat(material.correctVal, "correctVal");
        DebugUI::CheckFloat(material.viignetteVal, "viignetteVal");
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForBoxFilter& material)
{

#ifdef USE_IMGUI
    if (ImGui::TreeNode("BoxFilter")) {

        ImGui::DragFloat("kernel", &material.kernel, 1.0f, 0.0f, 1001.0f);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForGaussianFilter& material)
{

#ifdef USE_IMGUI
    if (ImGui::TreeNode("GaussianFilter")) {

        ImGui::DragInt("kernel", &material.kernel, 1, 1);
        ImGui::DragFloat("sigma", &material.sigma);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForLuminanceBasedOutline& material)
{

#ifdef USE_IMGUI
    if (ImGui::TreeNode("LuminanceBasedOutline")) {

        ImGui::DragFloat("weightVal", &material.weightVal, 0.1f);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForDepthBasedOutline& material)
{

#ifdef USE_IMGUI
    if (ImGui::TreeNode("DepthBasedOutline")) {
        ImGui::DragFloat("lineWidth", &material.lineWidth, 0.1f);
        DebugUI::ShowMatrix4x4(material.projectionInverse);
        ImGui::ColorEdit3("color", &material.color.x);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForRadialBlur& material)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("RadialBulr")) {

        ImGui::DragFloat2("center", &material.center.x, 0.01f, 0.0f, 1.0f);
        ImGui::DragInt("numSamples", &material.numSamples, 1.0f, 1);
        ImGui::DragFloat("blurWidth", &material.blurWidth, 0.01f);
        ImGui::TreePop();
    }
#endif
}
void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForDissolve& material)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Dissolve")) {
        //ImGui::Checkbox("useDissolve", &materialForDissolve_->useDissolve);
        ImGui::DragFloat("maskVal", &material.maskVal, 0.01f, 0.0f, 1.0f);
        ImGui::ColorEdit3("color", &material.rgb.x);
        ImGui::TreePop();
    }
#endif
}
void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForRandom& material)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Random")) {
        //ImGui::Checkbox("useRandom", &materialForRandom_->useRandom);
        ImGui::DragFloat("time", &material.time, 0.01f);
        ImGui::TreePop();
    }
#endif
}
void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForThermography& material)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Thermography")) {
        ImGui::DragFloat("alpha", &material.alpha);
        ImGui::DragInt("kernel", &material.kernel, 1, 1);
        ImGui::DragFloat("sigma", &material.sigma);
        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckPostEffectMaterial(PostEffectMaterial::MaterialForMosaic& material)
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Mosaic")) {
        ImGui::Checkbox("useMosaic", &material.useMosaic);
        ImGui::SliderInt("size", &material.size,0,2000);
        ImGui::SliderFloat("vol", &material.vol,0.0f,1.0f);
        ImGui::TreePop();
    }
#endif
}


void DebugUI::CheckObject3d(Object3d& object3d)
{
#ifdef USE_IMGUI
    if (!&object3d) {
        return;
    }

    auto* objectName = object3d.GetObjectName().c_str();
    static char tagBuffer[128] = "";
    static bool isInitialized = false;

    // 初回呼び出し時のみ、引数で渡された文字列をバッファにコピー
    if (!isInitialized && objectName != nullptr) {
        // 安全のためにバッファサイズを超えないようにコピー
        strncpy(tagBuffer, objectName, sizeof(tagBuffer) - 1);
        tagBuffer[sizeof(tagBuffer) - 1] = '\0'; // 終端ヌル文字を保証
        isInitialized = true;
    }

    std::string selectObjectName = "SelectObject :" + std::to_string(object3d.GetObjectID());

    if (ImGui::TreeNode(selectObjectName.c_str())) {

        //ファイルタグ名を入力
        if (ImGui::InputText("ObjectName", tagBuffer, IM_ARRAYSIZE(tagBuffer))) {
            object3d.SetObjectName(tagBuffer);
        };

        std::string currentObjectTypeName = object3d.GetObjectType();

        if (ImGui::BeginCombo("Set ObjectTypeName", currentObjectTypeName.c_str())) {
            // オブジェクト名を選択肢に入れる
            for (auto objectTypeName : LevelEditor::GetObjectTypeName()) {
                // 選択肢を表示（クリックされたら true を返す）
                if (ImGui::Selectable(objectTypeName.c_str(), true)) {
                    // クリックされたらオブジェクトタイプをセットする
                    object3d.SetObjectType(objectTypeName);
                    break;
                }
            }

            ImGui::EndCombo();
        }

        CheckWorldTransform(object3d.GetWorldTransform(), "WorldTransform");
        ShowMatrix4x4(object3d.GetWorldMatrix());
       
        //表示と非表示を切り替える
        bool disable = object3d.GetDisabled();
        ImGui::Checkbox("disable", &disable);
        object3d.SetDisabled(disable);

        auto& material = object3d.GetMaterial();

        CheckObject3dMaterial(
            material.color,
            material.lightMode,
            material.shininess,
            material.temperature,
            material.uvTransform,
            material.environmentCoefficient,
            material.maskVal,
            material.rgb,
            material.maskEdgeMin,
            material.maskEdgeMax,
            material.maskGamma,
            "Material");


        CheckTransform(object3d.GetUVTransform(), "UVTransfrom");
        CheckWaveData(object3d.GetWaveData(0), "Wave0");
        CheckWaveData(object3d.GetWaveData(1), "Wave1");
        CheckBalloonData(object3d.GetBalloonData());
        CheckMeltData(object3d.GetMeltData(), "meltData");

        auto* primitive = object3d.GetPrimitive();

        if (primitive) {
            
            //プリミティブならテクスチャをセットできる
            int textureIndex = object3d.GetTextureHandle();
            if (ImGui::SliderInt("texture", &textureIndex, 0, TextureFactory::TEXTURES)) {
                //テクスチャのセット
                object3d.SetTextureHandle(static_cast<TextureFactory::Handle>(textureIndex));
            };

            std::string currentModelName = "unknow";
            if (auto model = dynamic_cast<Model*>(primitive)) {

                for (const auto& [name, managerModel] : ModelManager::GetModels()) {
                    if (model == managerModel.get()) {
                        currentModelName = name.string().c_str();
                    }
                }
            }

            if (ImGui::BeginCombo("Set Model", currentModelName.c_str())) {
                // マップ内のすべてのシーンをループして選択肢を作る
                for (const auto& [name, scene] : ModelManager::GetModels()) {
                    // 選択肢を表示（クリックされたら true を返す）
                    if (ImGui::Selectable(name.string().c_str(), true)) {
                        // クリックされたらシーン切り替え関数を呼ぶ
                        object3d.SetMeshAndMaterial(ModelManager::GetModel(name));
                        break;
                    }
                }

                ImGui::EndCombo();
            }

            const char* topologyType[] =
            {
            "Plane",
            "Cube",
            "Sphere",
            "Ring",
            "Cylinder"
            };

            std::string currentPrimitiveName = primitive->GetMeshName();

            if (ImGui::BeginCombo("Set Primitive", currentPrimitiveName.c_str())) {
                // オブジェクト名を選択肢に入れる
                for (auto name : topologyType) {
                    // 選択肢を表示（クリックされたら true を返す）
                    if (ImGui::Selectable(name, true)) {
                        // クリックされたらセットする
                        object3d.SetMeshAndMaterial(PrimitiveFactory::GetPrimitiveForName(name));
                        break;
                    }
                }

                ImGui::EndCombo();
            }
        }

        if (auto* aniObj = dynamic_cast<AnimationObject3d*>(&object3d)) {

            if (ImGui::TreeNode("Animation")) {

                for (auto& [name, animations] : aniObj->GetAnimations()) {
                    ImGui::Text(name.c_str());
                    ImGui::SliderFloat("duration", &animations.duration, 0.0f, 1000000.0f);
                }

                ImGui::TreePop();
            }
        }


        // 現在トリガーに設定されている遷移先ステージ名を取得
        std::string currentStageName = object3d.GetNextStageName();

        if (ImGui::BeginCombo("Set StageName", currentStageName.c_str())) {
            // オブジェクト名を選択肢に入れる
            for (auto [stage, nextStageName] : StageManager::GetInstance()->GetStageNames()) {
                // 選択肢を表示（クリックされたら true を返す）
                if (ImGui::Selectable(nextStageName.c_str(), true)) {
                    // クリックされたらStageNameをセットする
                    object3d.SetNextStageName(nextStageName);
                    break;
                }
            }

            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }

    // 直前に描画した要素が右クリックされたら、ポップアップを開く
    if (ImGui::BeginPopupContextItem("ObjectContextMenu")) // "ObjectContextMenu"はポップアップの一意のID
    {
        if (ImGui::Selectable("UnregisterObject"))
        {
            ObjectManager::GetInstance()->UnregisterObject(&object3d);
        }
        ImGui::EndPopup();
    }
#endif
}
void DebugUI::CheckParticle(ParticleManager* particleManager)
{
#ifdef USE_IMGUI

    ParticleManager& particle = *particleManager;
    ImGui::Begin("Particle");

    if (ImGui::TreeNode("Particles")) {

        for (const auto& [name, group] : particle.GetParticleGroups()) {

            if (ImGui::TreeNode(name.c_str())) {

                ImGui::Checkbox("useModel", &group->useModel);
                ImGui::Checkbox("useSpriteCamera", &group->useSpriteCamera);
                auto& material = group->materialResource.data;
                CheckObject3dMaterial(
                    material->color, material->lightMode,
                    material->shininess,
                    material->temperature,
                    material->uvTransform, 
                    material->environmentCoefficient,
                    material->maskVal,
                    material->rgb,
                    material->maskEdgeMin,
                    material->maskEdgeMax,
                    material->maskGamma,
                    "Material");
                ImGui::SliderFloat2("textureSize", &group->textureSize.x, 0.0f, static_cast<float>(Window::GetClientWidth()));

                for (std::list<Particle>::iterator itr = group->particles.begin(); itr != group->particles.end(); ++itr) {
                    CheckTransform((*itr).transform, name.c_str());
                }
                ImGui::TreePop();
            }

        }
        ImGui::TreePop();
    }

    ImGui::End();
#endif
}

void DebugUI::CheckTransforms(Vector3& scale, Vector3& rotate, Vector3& translate, const char* label) {
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {
        ImGui::SliderFloat3("scale", &scale.x, 0.0f, 10.0f);
        ImGui::SliderFloat3("rotation", &rotate.x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("translation", &translate.x, -1000.0f, 1000.0f);
        ImGui::TreePop();
    }
#endif
};
void DebugUI::CheckColor(Vector4& color, const char* label) {
#ifdef USE_IMGUI
    ImGui::ColorEdit4(label, (float*)&color);
#endif
}

void DebugUI::CheckObject3dMaterial(
    Vector4& color,
    int32_t& lightMode,
    float& shininess,
    float& tempereture,
    Matrix4x4& uvMatrix,
    float& environmentCoefficient,
    float& maskVal,
    Vector3& dissolveRgb,
    float& maskEdgeMin,
    float& maskEdgeMax,

    float& gamma,
    const char* label) {
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {
        CheckColor(color, "color");
        CheckLightMode(lightMode, "lightMode");
        ImGui::SliderFloat("shininess", &shininess, 0.0f, 100.0f);
        ImGui::SliderFloat("temperature", &tempereture, 0.0f, 1.0f);
        ShowMatrix4x4(uvMatrix, "uvMatrix");
        ImGui::SliderFloat("environmentCoefficient", &environmentCoefficient, 0.0f, 1.0f);

        ImGui::DragFloat("maskVal", &maskVal, 0.01f, 0.0f, 1.0f);
        ImGui::ColorEdit3("maskEdigColor", &dissolveRgb.x);
        ImGui::SliderFloat("maskEdgeMin", &maskEdgeMin, 0.0f, maskEdgeMax);
        ImGui::SliderFloat("maskEdgeMax", &maskEdgeMax, maskEdgeMin, 1.0f);
        ImGui::SliderFloat("gamma", &gamma, 0.0f, 1000.0f);
        ImGui::TreePop();
    }
#endif
}


void DebugUI::CheckTransform(EulerTransform& transform, const char* label)
{
    CheckTransforms(transform.scale, transform.rotate, transform.translate, label);
}

void DebugUI::CheckWorldTransform(WorldTransform& worldTransform, const char* label) {

    CheckTransforms(worldTransform.eTransform_.scale, worldTransform.eTransform_.rotate, worldTransform.eTransform_.translate, label);

};

void DebugUI::CheckDirectionalLight() {
#ifdef USE_IMGUI

    if (ImGui::TreeNode("DirectionalLight")) {
        DirectionalLight* directionalLight = DirectionalLightManager::GetDirectionalLightData();
        Vector3 direction = directionalLight->direction;

        ImGui::ColorEdit4("color", &directionalLight->color.x);
        ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f);//後で正規化する
        directionalLight->direction = Normalize(direction);
        ImGui::DragFloat("intensity", &directionalLight->intensity);
        ImGui::TreePop();
    }

#endif
};

void DebugUI::CheckLightMode(int32_t& lightMode, const char* label) {
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {

        const char* lights[] = { "NONE", "LambertianReflectance", "HalfLambert" };
        int light_current = lightMode;

        if (ImGui::Combo("LightMode", &light_current, lights, IM_ARRAYSIZE(lights))) {
            lightMode = light_current % 3;
        };

        ImGui::TreePop();
    }
#endif
};

void DebugUI::CheckBlendMode(BlendMode& blendMode) {
#ifdef USE_IMGUI
    if (ImGui::TreeNode("BlendMode")) {

        const char* blendModes[] = {
            "kBlendModeNone",
            "kBlendModeNormal",
            "kBlendModeAdd",
            "kBlendModeSubtract",
            "kBlendModeMultiply",
            "kBlendModeScreen"
        };

        static int blendMode_current = 1;

        ImGui::Combo("blendMode", &blendMode_current, blendModes, IM_ARRAYSIZE(blendModes));
        blendMode = static_cast<BlendMode>(blendMode_current % 6);
        ImGui::TreePop();
    }
#endif
};

void DebugUI::ShowMainViewPort(CbvSrvUavDescriptorHeap* srvDescriptorHeap, const uint32_t srvIndex)
{
#ifdef USE_IMGUI
    // 1. 画面の中心座標を計算
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImVec2 screen_center = ImVec2(
        main_viewport->WorkPos.x + main_viewport->WorkSize.x * 0.5f,
        main_viewport->WorkPos.y + main_viewport->WorkSize.y * 0.5f
    );

    bool isTabTriggered = false;

    // 'tab' キーが押されたらサイズ変更のトリガーを引く
    if (Input::IsTriggerKey(DIK_TAB)) {
        useMaxGameWindow_ = !useMaxGameWindow_;
        isTabTriggered = true; // このフレームで切り替わったことを記録
    }

    if (useMaxGameWindow_) {
        // 【最大化のとき】
        // 画面の左上に配置し、画面全体のサイズ（WorkSize）を「常に(Always)」強制適用
        ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(main_viewport->WorkSize, ImGuiCond_Always);

        // 全画面表示っぽくするために、タイトルバー、移動、リサイズ、折りたたみを無効化
    } else {
        if (isTabTriggered) {
            // 戻った瞬間だけ 800x600にして画面中央へ
            ImGui::SetNextWindowPos(screen_center, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
            ImGui::SetNextWindowSize(ImVec2(640.0f, 360.0f), ImGuiCond_Always);
        }
    }

    // 3. ウィンドウの開始（リサイズ禁止フラグは付けない）
    if (ImGui::Begin("Game Window")) {

        float client_w = (float)Window::GetClientWidth();
        float client_h = (float)Window::GetClientHeight();

        // ゼロ除算を防ぐ安全対策
        if (client_h > 0.0f) {
            float target_aspect = client_w / client_h;

            // 2. 現在のImGuiウィンドウ内の「利用可能な最大サイズ」を取得
            ImVec2 avail_size = ImGui::GetContentRegionAvail();

            // 3. まず「横幅」を限界まで広げた場合のサイズを計算
            float game_w = avail_size.x;
            float game_h = game_w / target_aspect;

            // 4. もし縦幅がウィンドウからはみ出てしまう場合は、「縦幅」を限界まで広げる計算に切り替える
            if (game_h > avail_size.y) {
                game_h = avail_size.y;
                game_w = game_h * target_aspect;
            }

            // 5. 【重要】余ったスペースを計算して、ゲーム画面を中央に寄せる（レターボックス化）
            float offset_x = (avail_size.x - game_w) * 0.5f;
            float offset_y = (avail_size.y - game_h) * 0.5f;

            // ImGuiの描画カーソル位置を、計算した余白の分だけズラす
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offset_x, ImGui::GetCursorPosY() + offset_y));


            // SrvManager から GPUハンドルを取得
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvDescriptorHeap->GetGPUDescriptorHandle(srvIndex);

            // 画像の表示 (引数: テクスチャID, 表示サイズ(横, 縦))
            ImGui::Image((ImTextureID)gpuHandle.ptr, { game_w,game_h });

            // テスト用のダミー枠（実際は上の ImGui::Image などに置き換えてください）
            ImGui::Text("GameScreenSize %.0fx%.0f", game_w, game_h);

        }
    }

    ImGui::End();
#endif
}

void DebugUI::CheckCaracterState(CharacterState& characterState, const char* label)
{
#ifdef USE_IMGUI

    if (ImGui::TreeNode(label)) {

        ImGui::SliderInt("Maxhp", &characterState.hps.maxHp, 0, 500);
        ImGui::SliderInt("hp", &characterState.hps.hp, 0, characterState.hps.maxHp);

        ImGui::Checkbox("isDead", &characterState.isDead);
        ImGui::Checkbox("isHit", &characterState.isHit);

        ImGui::TreePop();
    }

#endif

}

void DebugUI::CheckFPS() {
#ifdef USE_IMGUI
    ImGui::Begin("Debug");
    ImVec4 color = (ImGui::GetIO().Framerate < 55.0f) ? ImVec4(1, 0, 0, 1) : ImVec4(0, 1, 0, 1);
    ImGui::TextColored(color, "FPS : %0.1f", ImGui::GetIO().Framerate);
    ImGui::TextColored(color, "DeltaTime : %f", TimeManager::DeltaTime());
    ImGui::End();
#endif
}

void DebugUI::CheckSound()
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Sound")) {

        ImGui::SliderFloat("SE Val", &Sound::seVolume_, 0.0f, 1.0f);
        ImGui::SliderFloat("BGM Val", &Sound::bgmVolume_, 0.0f, 1.0f);

        static bool isLoop = false;
        static float volume = false;
        ImGui::Checkbox("isLoop", &isLoop);
        ImGui::SliderFloat("volume", &volume, 0.0f, 10.0f);


        for (auto& [filename, sound] : Sound::GetSouneDatas()) {

            if (ImGui::TreeNode(filename.filename().string().c_str())) {

                ImGui::Text("fileName : %s", filename.string().c_str());

                ImGui::Text("IsPlaying? : %s", Sound::IsPlaying(filename) ? "Playing!!" : "NoSound");

                if (ImGui::Button("Play")) {
                    Sound::Play(filename, volume, isLoop);
                }

                if (ImGui::Button(" Stop")) {
                    Sound::Stop(filename);
                }

                if (ImGui::Button("Pause")) {
                    Sound::Pause(filename);
                }

                // 正規化済みのモノラル波形バッファ
                if (ImGui::TreeNode("Show Oscilloscope")) {
                    static int writeIdx = 0;
                    std::vector<float> waveform = Sound::GetWaveform(filename);
                    writeIdx = (int)(Sound::GetSamplesPlayed(filename) % waveform.size());

                    float scale = Sound::bgmVolume_; // 0.0〜1.0

                    ImGui::PlotLines("", waveform.data(), (int)waveform.size(), writeIdx,
                        nullptr, -scale, scale, ImVec2(0, 64));
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

        };

        ImGui::TreePop();
    };
#endif

}

void DebugUI::CheckFlag(bool& flag, const char* label)
{
#ifdef USE_IMGUI
    std::string labels = std::string(label) + " : " + (flag ? "true" : "false");
    ImGui::Text("%s", labels.c_str());
#endif
}

void DebugUI::SwitchFlag(bool& flag, const char* label)
{
#ifdef USE_IMGUI
    CheckFlag(flag, label);

    if (ImGui::Button(label)) {
        // スペースキーを押すとデバッグカメラに切り替える
        flag = flag ? false : true;
    }
#endif

}
void DebugUI::Button(const char* label, std::function<void()> onSwitch)
{
#ifdef USE_IMGUI
    if (ImGui::Button(label)) {
        if (onSwitch) {
            onSwitch(); // ボタンが押されたら関数オブジェクトを実行！
        }
    }
#endif
}
#include"PostProcessManager/PostProcessManager.h"

void DebugUI::CheckPostEffect()
{

#ifdef USE_IMGUI
    auto* postProcessManager = PostProcessManager::GetInstance();

    if (ImGui::TreeNode("PostEffect Model")) {

        auto* modelMaterial = postProcessManager->GetPostEffectMaterial(PostProcessManager::kModel);

        CheckPostEffectMaterial(*modelMaterial->GetMaterialGaussianFilter());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialGrayScale());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForVignette());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForBoxFilter());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForLuminanceBasedOutline());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForDepthBasedOutline());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForRadialBlur());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForDissolve());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialThermography());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForRandom());
        CheckPostEffectMaterial(*modelMaterial->GetMaterialForMosaic());
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("PostEffect Sprite")) {

        auto* spritelMaterial = postProcessManager ->GetPostEffectMaterial(PostProcessManager::kSprite);

        CheckPostEffectMaterial(*spritelMaterial->GetMaterialGaussianFilter());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialGrayScale());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForVignette());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForBoxFilter());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForLuminanceBasedOutline());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForDepthBasedOutline());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForRadialBlur());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForDissolve());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialThermography());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForRandom());
        CheckPostEffectMaterial(*spritelMaterial->GetMaterialForMosaic());
        ImGui::TreePop();
    }

#endif

}

