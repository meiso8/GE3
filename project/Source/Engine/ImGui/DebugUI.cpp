#define NOMINMAX
#include "DebugUI.h"
#include"CharacterState.h"

#include"MyEngine.h"
#include"Input.h"
#include"Sprite.h"
#include"MeshCommon.h"
#include"Model.h"
#include"Particle/Particle.h"
#include"Particle/ParticleEmitter.h"
#include"Object3d.h"
#include"AnimationObject3d.h"
#include"Sound.h"

#include"Lights/Light.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"Lights/PointLightManager.h"
#include"Engine/Mesh/Font/Font.h"

#include"PSO.h"
#include"Camera.h"
#include"JsonFile.h"

#include<numbers>
#include<algorithm>

struct Param {
    char name[128];
    char value[128];
};


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

void DebugUI::CheckCamera(Camera& camera, const std::string label) {

#ifdef USE_IMGUI
    if (ImGui::TreeNode(label.c_str())) {

        CheckTransforms(camera.scale_, camera.rotate_, camera.translate_, "worldMatrix");
        ShowMatrix4x4(camera.worldMat_);
        if (ImGui::Button("InitTransform")) {
            camera.InitializeTransform();
        }
        if (ImGui::TreeNode("ShericalCoordinate")) {
            ImGui::SliderFloat("polar", &camera.sphericalCoordinate_.polar, -10.0f, 10.0f);
            ImGui::SliderFloat("azimuthal", &camera.sphericalCoordinate_.azimuthal, -10.0f, 10.0f);
            ImGui::SliderFloat("radius", &camera.sphericalCoordinate_.radius, -100.0f, 100.0f);
            ImGui::TreePop();
        }

        ImGui::SliderFloat2("ofsset", &camera.offset_.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("nearZ", &camera.nearZ_, 0.0f, 1000.0f);
        ImGui::SliderFloat("farZ", &camera.farZ_, 0.0f, 1000.0f);
        ImGui::SliderFloat("fovAngleY", &camera.fovAngleY_, -6.28f, 6.28f);
        ImGui::Text("Type : %s", (camera.projectionType_ == Camera::PERSPECTIVE) ? "PERSPECTIVE" : "PARALLEL");

        if (ImGui::Button("ChangeType")) {
            camera.projectionType_ = (camera.projectionType_ == Camera::PERSPECTIVE) ? Camera::PARALLEL : Camera::PERSPECTIVE;
        }

        ImGui::TreePop();

    }
#endif
}


void DebugUI::CheckJsonFile()
{
#ifdef USE_IMGUI

    if (ImGui::TreeNode("Json")) {

        if (ImGui::TreeNode("CreateNewFile")) {

            static char tagBuffer[128] = "";
            ImGui::InputText("FileTag", tagBuffer, IM_ARRAYSIZE(tagBuffer));

            if (ImGui::Button("Create")) {
                // 新しい構造化JSONを作成
                nlohmann::json newJson;
                // 管理マップに登録
                JsonFile::SetJson(tagBuffer, newJson);
                JsonFile::MarkModified(tagBuffer);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("FindTag")) {

            // 安定した文字列保持用
            static std::vector<std::string> tagStrings;
            static std::vector<const char*> tagOptions;

            tagStrings.clear();
            tagOptions.clear();

            for (const auto& [tag, data] : JsonFile::GetJsonData()) {
                tagStrings.push_back(tag); // std::string を保持
            }

            for (const auto& str : tagStrings) {
                tagOptions.push_back(str.c_str()); // 安定したポインタを取得
            }

            // ImGui::Combo に渡す
            static int tag_current = 0;
            if (ImGui::Combo("Tags", &tag_current, tagOptions.data(), static_cast<int>(tagOptions.size()))) {
                ImGui::Text("Tag: %s", tagOptions[tag_current]);
            }

            nlohmann::json& jsonFile = JsonFile::GetJsonFiles(tagOptions[tag_current]);

            ImGui::Separator();
            if (ImGui::TreeNode("ShowJsonData")) {
                ImGui::Text("Name: %s", tagOptions[tag_current]);
                ImGui::TextWrapped("Data: %s", jsonFile.dump(2).c_str());
                ImGui::TreePop();
                ImGui::Separator();
            }

            static char structName[128] = "structName";
            static std::vector<Param> params = {};

            if (ImGui::TreeNode("AddParam")) {

                if (ImGui::InputText("StructName", structName, IM_ARRAYSIZE(structName))) {
                    JsonFile::ClearModified(tagOptions[tag_current]);
                }

                for (size_t i = 0; i < params.size(); ++i) {
                    if (ImGui::TreeNode(("param" + std::to_string(i)).c_str())) {

                        if (ImGui::InputText(("Name##" + std::to_string(i)).c_str(), params[i].name, IM_ARRAYSIZE(params[i].name))) {
                            JsonFile::ClearModified(tagOptions[tag_current]);
                        }

                        if (ImGui::InputText("Value##", params[i].value, IM_ARRAYSIZE(params[i].value))) {
                            JsonFile::ClearModified(tagOptions[tag_current]);
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
                    JsonFile::ClearModified(tagOptions[tag_current]);
                }


                ImGui::TreePop();
            }

            if (ImGui::Button("Save")) {

                for (const auto& param : params) {
                    jsonFile[structName][param.name] = param.value;
                }
                // ファイル保存
                JsonFile::SaveJson(tagOptions[tag_current]);
                JsonFile::MarkModified(tagOptions[tag_current]);

            }

            // 保存完了メッセージを表示
            if (JsonFile::IsModified(tagOptions[tag_current])) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "File saved");
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "File not saved.");
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
#endif
}

void DebugUI::CheckSRVIndex() {
#ifdef USE_IMGUI
    static int index = 0;

    ImGui::Begin("SRVTexture");
    // 例：表示したいSRVのインデックス番号
    // （テクスチャを読み込んだ時のインデックスや、RenderTextureのsrvIndexなど）
    ImGui::SliderInt("srvIndex", &index, 0, SrvManager::kMaxSRVCount - 1);

    // SrvManager から GPUハンドルを取得
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = SrvManager::GetGPUDescriptorHandle(index);

    // ImGui::Imageに渡すために ImTextureID (void* 型) にキャストする
    ImTextureID texID = reinterpret_cast<ImTextureID>(gpuHandle.ptr);

    // 画像の表示 (引数: テクスチャID, 表示サイズ(横, 縦))
    ImGui::Image(texID, ImVec2(128.0f, 72.0f));

    ImGui::End();
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

void DebugUI::CheckModel(Model& model, const char* label) {
#ifdef USE_IMGUI
    ImGui::Begin("Model");
    auto* modelData = model.GetModelData();
    std::string string = "filePath :" + modelData->filePath;
    ImGui::Text(string.c_str());
    ImGui::Text("indices : Size : %d", modelData->indices.size());
    ImGui::Text("vertices : Size : %d", modelData->vertices.size());

    if (ImGui::TreeNode("material")) {
        for (auto& [name, materials] : modelData->materials) {
            if (ImGui::TreeNode(name.c_str())) {
                for (int i = 0; i < materials.textureData_.size(); ++i) {
                    std::string textureFilePath = "material : FilePath :" + materials.textureData_[i].textureFilePath;
                    ImGui::Text(textureFilePath.c_str());
                    ImGui::Text("material : SrvIndex : %d", materials.textureData_[i].textureSrvIndex);
                }
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }


    ImGui::End();
#endif
}


void DebugUI::CheckInput() {
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Input")) {
        ImGui::Begin("Input");
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
    ImGui::Begin("Font");

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



    ImGui::End();
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
void DebugUI::CheckObject3d(Object3d& object3d, const char* label)
{
#ifdef USE_IMGUI
    ImGui::Begin("Object3d");

    if (ImGui::TreeNode(label)) {
        CheckWorldTransform(object3d.worldTransform_, label);
        ShowMatrix4x4(object3d.worldTransform_.matWorld_);

        CheckMaterial(object3d.GetMaterial(), "material");
        CheckTransform(object3d.GetUVTransform(), "uvTransfrom");

        CheckWaveData(object3d.GetWaveData(0), "wave0");
        CheckWaveData(object3d.GetWaveData(1), "wave1");
        CheckBalloonData(object3d.GetBalloonData());

        if (auto* aniObj = dynamic_cast<AnimationObject3d*>(&object3d)) {
            if (ImGui::TreeNode("Animation")) {
                for (auto& [name, animations] : aniObj->GetAnimations()) {
                    ImGui::Text(name.c_str());
                    ImGui::SliderFloat("duration", &animations.duration, 0.0f, 1000000.0f);
                }
                ImGui::TreePop();
            }

        }

        ImGui::TreePop();
    }
    ImGui::End();
#endif
}
void DebugUI::CheckParticle(ParticleEmitter& particleEmitter, const char* label)
{
#ifdef USE_IMGUI

    ParticleManager& particle = *ParticleManager::GetInstance();
    ImGui::Begin("Particle");

    static  Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
    Emitter& emitter = particleEmitter.GetEmitter();

    if (ImGui::TreeNode(label)) {
        int movement = static_cast<int>(emitter.movement);
        ImGui::SliderInt("movement", &movement, 0, 2);
        emitter.movement = static_cast<ParticleMovements>(movement);
        ImGui::SliderFloat("radius", &emitter.radius, 0.1f, 10.0f);
        ImGui::SliderFloat("lifeTime", &emitter.lifeTime, -1.0f, 50.0f);

        ImGui::SliderFloat3("scaleAABBMin", &emitter.scaleAABB_.min.x, -20.0f, 0.0f);
        ImGui::SliderFloat3("scaleAABBMax", &emitter.scaleAABB_.max.x, 0.0f, 20.0f);

        ImGui::SliderFloat3("translateMin", &emitter.translateAABB_.min.x, -20.0f, 0.0f);
        ImGui::SliderFloat3("translateMax", &emitter.translateAABB_.max.x, 0.0f, 20.0f);

        ImGui::SliderFloat3("rotateAABBMin", &emitter.rotateAABB_.min.x, -20.0f, 0.0f);
        ImGui::SliderFloat3("rotateAABBMax", &emitter.rotateAABB_.max.x, 0.0f, 20.0f);

        ImGui::SliderFloat3("velcityAABBMax", &emitter.velocityAABB.min.x, -20.0f, 0.0f);
        ImGui::SliderFloat3("velcityAABBMin", &emitter.velocityAABB.max.x, 0.0f, 20.0f);


        CheckBlendMode(emitter.blendMode);
        CheckColor(emitter.color, "color");

        int count = emitter.count;
        ImGui::SliderInt("createNum", &count, 0, particle.kNumMaxInstance);
        emitter.count = count;

        CheckWorldTransform(emitter.transform, "transform");
        ImGui::Text("frequencyTime : %f", emitter.frequencyTime);
        ImGui::SliderFloat("frequency", &emitter.frequency, 0.1f, 10.0f);
        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Particles")) {

        for (const auto& [name, group] : particle.GetParticleGroups()) {

            if (ImGui::TreeNode(name.c_str())) {

                ImGui::Checkbox("useModel", &group->useModel);
                ImGui::Checkbox("useBillboard", &group->useBillboard);
                ImGui::Checkbox("useSpriteCamera", &group->useSpriteCamera);

                ImGui::SliderFloat3("acceleration", &group->accelerationField.acceleration.x, -100.0f, 100.0f);
                ImGui::SliderFloat3("area.min", &group->accelerationField.area.min.x, -100.0f, 0.0f);
                ImGui::SliderFloat3("area.max", &group->accelerationField.area.max.x, 0.0f, 100.0f);
                ImGui::SliderFloat2("textureSize", &group->textureSize.x, 0.0f, static_cast<float>(Window::GetClientWidth()));

                if (ImGui::Button(name.c_str())) {
                    particle.Emit(emitter);
                }
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
    ImGui::ColorEdit4("color", (float*)&color);
#endif
}

void DebugUI::CheckMaterial(Material& material, const char* label) {
#ifdef USE_IMGUI
    if (ImGui::TreeNode(label)) {
        CheckColor(material.color, "color");
        CheckLightMode(material.lightMode, "lightMode");
        ImGui::SliderFloat("shininess", &material.shininess, 0.0f, 100.0f);
        ImGui::SliderFloat("temperature", &material.temperature, 0.0f, 1.0f);
        ShowMatrix4x4(material.uvTransform, "uvMatrix");
        ImGui::SliderFloat("environmentCoefficient", &material.environmentCoefficient, 0.0f, 1.0f);

        ImGui::TreePop();
    }
#endif
}


void DebugUI::CheckTransform(EulerTransform& transform, const char* label)
{
    CheckTransforms(transform.scale, transform.rotate, transform.translate, label);
}

void DebugUI::CheckWorldTransform(WorldTransform& worldTransform, const char* label) {

    CheckTransforms(worldTransform.scale_, worldTransform.rotate_, worldTransform.translate_, label);

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
    if (ImGui::TreeNode("FPS")) {

        ImVec4 color = (ImGui::GetIO().Framerate < 55.0f) ? ImVec4(1, 0, 0, 1) : ImVec4(0, 1, 0, 1);
        ImGui::TextColored(color, "FPS : %0.1f", ImGui::GetIO().Framerate);
        ImGui::TextColored(color, "DeltaTime : %f", DirectXCommon::GetDeltaTime());
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "kDeltaTime : %f", 1.0f / 60.0f);

        ImGui::TreePop();
    }


#endif
}

void DebugUI::CheckSound()
{
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Sound")) {

        ImGui::SliderFloat("SE Val", &Sound::seVolume_, 0.0f, 1.0f);
        ImGui::SliderFloat("BGM Val", &Sound::bgmVolume_, 0.0f, 1.0f);

        // 正規化済みのモノラル波形バッファ
        if (ImGui::TreeNode("ShowOscilloscope")) {
            static int writeIdx = 0;
            std::vector<float> waveform = Sound::GetWaveform(SoundFactory::BGM_ArabRuins);
            writeIdx = (int)(Sound::GetSamplesPlayed(SoundFactory::BGM_ArabRuins) % waveform.size());

            float scale = Sound::bgmVolume_; // 0.0〜1.0

            ImGui::PlotLines("", waveform.data(), (int)waveform.size(), writeIdx,
                nullptr, -scale, scale, ImVec2(0, 64));
            ImGui::TreePop();
        }
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

