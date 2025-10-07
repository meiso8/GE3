#include "DebugUI.h"
#include"Normalize.h"
#include"Input.h"
#include"Sprite.h"
#include"SphereMesh.h"
#include"DirectionalLight.h"
#include"PSO.h"
#include"Camera/Camera.h"
#include"MyEngine.h"

#include<numbers>
#include<algorithm>

void DebugUI::CheckDirectionalLight(int& lightType) {

    DirectionalLight* directionalLight = MyEngine::GetDirectionalLightData();

    Vector3 direction = directionalLight->direction;
    ImGui::Begin("DirectionalLight");
    ImGui::ColorEdit4("color", &directionalLight->color.x);
    ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f);//後で正規化する
    directionalLight->direction = Normalize(direction);
    ImGui::DragFloat("intensity", &directionalLight->intensity);

    const char* lights[] = { "NONE", "LambertianReflectance", "HalfLambert" };

    static int light_current = 0;

    ImGui::Combo("LightMode", &light_current, lights, IM_ARRAYSIZE(lights));
    lightType = light_current % 3;
    ImGui::End();

};

void DebugUI::CheckBlendMode(uint32_t& blendMode) {

    ImGui::Begin("BlendMode");
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
    blendMode = blendMode_current % 6;
    ImGui::End();

};

void DebugUI::CheckFPS() {
    ImGui::Begin("Debug");
    ImGui::Text("FPS : %f", ImGui::GetIO().Framerate);
    ImGui::End();
}

void DebugUI::CheckInt(int& value) {

    ImGui::Begin("Value");
    ImGui::SliderInt("int", &value, -100, 100);
    ImGui::End();
};



void DebugUI::CheckCamera(Camera& camera) {
    ImGui::Begin("Camera");

    ImGui::SliderFloat3("translate", &camera.GetTranslate().x, -100.0f, 100.0f);
    ImGui::SliderFloat3("rotate", &camera.GetRotate().x, -100.0f, 100.0f);
    ImGui::SliderFloat3("scale", &camera.GetScale().x, -100.0f, 100.0f);

    ImGui::End();

}

void DebugUI::CheckModel(Model& model) {

    ImGui::Begin("Model");

    if (ImGui::TreeNode("Wave")) {
        Wave waveData1 = model.GetWaveData(0);
        ImGui::DragFloat("time", &waveData1.time, 0.03f);
        ImGui::DragFloat("amplitude", &waveData1.amplitude, 0.03f);
        ImGui::DragFloat3("direction", &waveData1.direction.x, 0.03f, 0.0f, 1.0f);
        ImGui::SliderFloat("frequency", &waveData1.frequency, 1.0f, 10.0f);
        Vector3 waveDirection1 = waveData1.direction;
        waveData1.direction = Normalize(waveDirection1);
        ImGui::TreePop();

    }

 

    if (ImGui::TreeNode("Wave2")) {
        Wave waveData2 = model.GetWaveData(1);

        ImGui::DragFloat("time", &waveData2.time, 0.03f);
        ImGui::DragFloat("amplitude", &waveData2.amplitude, 0.03f);
        ImGui::DragFloat3("direction", &waveData2.direction.x, 0.03f, 0.0f, 1.0f);
        ImGui::SliderFloat("frequency", &waveData2.frequency, 1.0f, 10.0f);
        Vector3 waveDirection2 = waveData2.direction;
        waveData2.direction = Normalize(waveDirection2);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Expansion")) {

        Balloon balloon = model.GetExpansionData();

        ImGui::DragFloat("expansionData", &balloon.expansion, 0.03f, 0.0f, 10.0f);
        ImGui::DragFloat("sphere", &balloon.sphere, 0.03f, 0.0f, 1.0f);
        ImGui::DragFloat("cube", &balloon.cube, 0.03f, 0.0f, 1.0f);
        ImGui::Checkbox("isSphere", &balloon.isSphere);

        ImGui::TreePop();
    }


    if (ImGui::TreeNode("UV")) {
        Transform uvTransform = model.GetUVTransform();
        Vector4 color = model.GetColor();
        ImGui::SliderFloat3("Translate", &uvTransform.translate.x, -100.0f, 100.0f);
        ImGui::SliderFloat3("Rotation", &uvTransform.rotate.x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("Scale", &uvTransform.scale.x, 0.0f, 100.0f);
        ImGui::ColorEdit4("color", &color.x);
        ImGui::TreePop();
    }

    ImGui::End();

}

void DebugUI::CheckInput(Input& input) {

    ImGui::Begin("Input");
    ImGui::SliderFloat("polar", &input.GetSc().polar, -10.0f, 10.0f);
    ImGui::SliderFloat("azimuthal", &input.GetSc().azimuthal, -10.0f, 10.0f);
    ImGui::SliderFloat("radius", &input.GetSc().radius, -100.0f, 100.0f);
    ImGui::SliderFloat2("startPos", &input.GetOffset().x, -100.0f, 100.0f);
    ImGui::SliderFloat2("currentPos", &input.GetCurrentPos().x, -100.0f, 100.0f);

    float x = 100;
    float y = 100;
    input.GetJoyStickPos(&x, &y, Input::BUTTON_LEFT);
    ImGui::Text("normLX:%f %f", x, y);
    float dX = 100;
    float dY = 100;
    input.GetJoyStickDPadButton(&dX, &dY);
  
    ImGui::Text("Dpad:%f %f", dX, dY);

    ImGui::Text("joyStateLX: %ld", input.GetJoyState().lX);//x軸位置
    ImGui::Text("joyStateLY: %ld", input.GetJoyState().lY);
    ImGui::Text("joyStateLZ: %ld", input.GetJoyState().lZ);
    ImGui::Text("joyStateRX: %ld", input.GetJoyState().lRx);//右スティック
    ImGui::Text("joyStateRY: %ld", input.GetJoyState().lRy);

    if (input.GetJoyState().rgdwPOV[0] != -1) {//十字キー　角度を表す
        ImGui::Text("POV[%d]: %lu", 0, input.GetJoyState().rgdwPOV[0]);
    } else {
        ImGui::Text("POV[%d]: Centered", 0);
    }



    for (int i = 0; i < 12; ++i) {
        ImGui::Text("Button[%d]: %s", i,
            (input.GetJoyState().rgbButtons[i] & 0x80) ? "Pressed" : "Released");
    }

    ImGui::End();

}

void DebugUI::CheckSprite(Sprite& sprite) {
    ImGui::Begin("Sprite");


    if (ImGui::TreeNode("transform")) {
        ImGui::SliderFloat2("pos", &sprite.GetPositionRef().x, -1280.0f, 1280.0f);
        ImGui::SliderFloat("rotation", &sprite.GetRotateRef(), 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("scale", &sprite.GetScaleRef().x, 0.0f, 10.0f);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("uvTransform")) {
        ImGui::SliderFloat3("uvTranslate", &sprite.GetUVTranslate().x, -100.0f, 100.0f);
        ImGui::SliderFloat3("uvRotation", &sprite.GetUVRotate().x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("uvScale", &sprite.GetUVScale().x, 0.0f, 100.0f);
        ImGui::TreePop();
    }
    ImGui::End();


}

void DebugUI::CheckSphere(SphereMesh& sphere) {
    ImGui::Begin("Sphere");


    if (ImGui::TreeNode("worldTransform")) {
        ImGui::SliderFloat3("translation", &sphere.GetTranslate().x, 0.0f, 6400.0f);
        ImGui::SliderFloat3("rotation", &sphere.GetRotate().x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("scale", &sphere.GetScale().x, 0.0f, 1000.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("uvTransform")) {
        ImGui::SliderFloat3("uvTranslate", &sphere.GetUVTransform().translate.x, -100.0f, 100.0f);
        ImGui::SliderFloat3("uvRotation", &sphere.GetUVTransform().rotate.x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("uvScale", &sphere.GetUVTransform().scale.x, 0.0f, 100.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Expansion")) {
        ImGui::DragFloat("expansionData", &sphere.GetExpansionData().expansion, 0.03f, 0.0f, 10.0f);
        ImGui::DragFloat("sphere", &sphere.GetExpansionData().sphere, 0.03f, 0.0f, 1.0f);
        ImGui::DragFloat("cube", &sphere.GetExpansionData().cube, 0.03f, 0.0f, 1.0f);
        ImGui::Checkbox("isSphere", &sphere.GetExpansionData().isSphere);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Color")) {
        ImGui::ColorEdit4("color", &sphere.GetColor().x);
        ImGui::TreePop();
    }

    ImGui::End();
}

void DebugUI::CheckWorldMatrix(Vector3& scale, Vector3& rotate, Vector3& translate, const char* label) {

    if (ImGui::TreeNode(label)) {
        ImGui::SliderFloat3("translation", &translate.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("rotation", &rotate.x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("scale", &scale.x, 0.0f, 10.0f);
        ImGui::TreePop();
    }

};

void DebugUI::CheckWorldTransform(WorldTransform& worldTransform) {

    //ImGui::Begin("WorldTransform");

    if (ImGui::TreeNode("worldTransform")) {
        ImGui::SliderFloat3("translation", &worldTransform.translate_.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("rotation", &worldTransform.rotate_.x, 0.0f, std::numbers::pi_v<float>*2.0f);
        ImGui::SliderFloat3("scale", &worldTransform.scale_.x, 0.0f, 10.0f);
        ImGui::TreePop();
    }
    //ImGui::End();
};

void DebugUI::CheckColor(Vector4& color) {
    ImGui::Begin("Color");
    ImGui::ColorEdit4("color", (float*)&color);
    ImGui::End();
}

