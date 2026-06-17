#include "ImGuiClass.h"
#include"CommandList.h"
#include"SRVmanager/SrvManager.h"
#include"ImGuizmo.h"
#include"Camera.h"
#include"Object3d.h"
#include"MakeMatrix.h"
#include <shellapi.h> // DragQueryFile用
#include"ModelManager.h"
#include"Texture.h"
#include<filesystem>
#include<Sound.h>
#include"Log.h"
#include"Texture.h"
#include"DebugUI.h"

namespace ImGuiLoadFile {
    std::filesystem::path droppedFilePath = "";
    bool isFileDropped = false;
}


#ifdef USE_IMGUI
void ImGuiClass::Initialize(Window& window,
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    SwapChain& swapChain,
    RenderTargetView& rtv) {

    uint32_t srvIndex = SrvManager::Allocate();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.38f, 0.56f, 0.83f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.62f, 0.55f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.99f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab] = ImVec4(0.34f, 0.34f, 0.68f, 0.79f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.40f, 0.73f, 0.84f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.28f, 0.28f, 0.57f, 0.82f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.35f, 0.35f, 0.65f, 0.84f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);



    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8;
    style.FrameRounding = 8;
    style.GrabRounding = 8;

    ImGui_ImplWin32_Init(window.GetHwnd());
    ImGui_ImplDX12_Init(device.Get(),
        swapChain.GetDesc().BufferCount,
        rtv.GetDesc().Format,
        SrvManager::GetDescriptorHeap(),
        SrvManager::GetCPUDescriptorHandle(srvIndex),
        SrvManager::GetGPUDescriptorHandle(srvIndex));


    // ==========================================
    // ここに解決のためのコードを追加
    // ==========================================
    ImGuiIO& io = ImGui::GetIO();
    // 1. 強制的にフォントをビルド

    // 日本語フォントを読み込む場合（※パスは環境に合わせて調整してください）
    //io.Fonts->AddFontFromFileTTF("Resources\Fonts\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    io.Fonts->Build();
    // 2. DX12バックエンドに、フォントテクスチャをGPUへアップロードさせる
    ImGui_ImplDX12_CreateDeviceObjects();
    // ==========================================

    //ドラッグ可能にする
    DragAcceptFiles(window.GetHwnd(), TRUE);
}

void ImGuiClass::FrameStart() {


    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::SetOrthographic(false); // パースペクティブカメラの場合

}

void ImGuiClass::Render() {

    //ImGuiの内部コマンドを生成する
    ImGui::Render();

}

void ImGuiClass::DrawImGui(ID3D12GraphicsCommandList* commandList) {

    //諸々の描画処理が終了下タイミングでImGuiの描画コマンドを積む
//実際のcommandListのImGuiの描画コマンドを積む
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

}

void ImGuiClass::ShutDown() {

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}



void ConvertMatArray(const Matrix4x4& srcMatrix, float dstArray[16])
{
    // 行列のメモリ構造をそのまま16個のfloat配列にコピー
    std::memcpy(dstArray, &srcMatrix.m[0][0], sizeof(float) * 16);

}

void ImGuiClass::UpdateGuizmo(Camera& camera, Object3d& targetObject)
{



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

    /*
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
    */

    float viewMat[16] = { 0.0f };
    float projectionMat[16] = { 0.0f };
    float worldMat[16] = { 0.0f };


    ConvertMatArray(camera.GetViewMatrix(), viewMat);
    ConvertMatArray(camera.GetProjectionMatrix(), projectionMat);
    ConvertMatArray(targetObject.worldTransform_.matWorld_, worldMat);

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
            &targetObject.worldTransform_.translate_.x,
            &targetObject.worldTransform_.rotate_.x, // ラジアン（または度、ライブラリの仕様に合わせて調整）
            &targetObject.worldTransform_.scale_.x
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

#endif

}

void ImGuiClass::DropFiles(WPARAM wParam) {
    HDROP hDrop = (HDROP)wParam;

    // ImGuiが「今、マウスの下にImGuiのウィンドウがあるよ」と言っているか判定
    // かつ、特定のウィンドウ（例: "Model Loader"）の上にいるかチェックしたい場合
    if (ImGui::GetCurrentContext() && ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {

        // ドロップされたファイルの数を取得（今回は1つだけ処理）
        UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
        if (fileCount > 0) {
            wchar_t filePath[MAX_PATH];
            // 最初のファイルのパスを取得
            DragQueryFileW(hDrop, 0, filePath, MAX_PATH);

            // パスを保存して、メインループ側でロードさせる
            ImGuiLoadFile::droppedFilePath = filePath;
            ImGuiLoadFile::isFileDropped = true;
        }
    }
    DragFinish(hDrop);
}


void HandleDroppedFile(const std::filesystem::path& fullPath) {
    std::string directoryPath = fullPath.parent_path().string() + "/";
    std::string filename = fullPath.filename().string();
    std::string ext = fullPath.extension().string();
    // すべて小文字に変換
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 拡張子ごとのロード処理
    if (ext == ".gltf" || ext == ".obj") {
        ModelManager::LoadModel(fullPath);
    } else if (ext == ".png" || ext == ".jpeg" || ext == ".jpg" || ext == ".dds") {
        // ─── テクスチャのロード ───
        Texture::AddTextureHandle(fullPath);
    } else if (ext == ".mp3" || ext == ".wav") {
        // ─── サウンドのロード ───
        //一旦これにしておくが後で変更する
        Sound::Load(fullPath);
    } else {
        // 対応していない拡張子の場合
        std::string message = "未対応のファイル形式です: " + ext;
        LogFile::Log(message);
        ImGui::Text(message.c_str());
    }

}

void ImGuiClass::DrawModelLoaderWindow()
{
 
    ImGui::Begin("Assets");

    // ファイルがドロップされたらDX12のリソースを生成
    if (ImGuiLoadFile::isFileDropped) {
        
        std::filesystem::path fullPath(ImGuiLoadFile::droppedFilePath);

        HandleDroppedFile(fullPath);

        ImGuiLoadFile::isFileDropped = false; // フラグを下ろす
    }

    ImGui::Text("%s", ImGuiLoadFile::droppedFilePath.string().c_str());

    DebugUI::CheckTextures();
    DebugUI::CheckModels();
    DebugUI::CheckSound();

    ImGui::End();
}

#endif