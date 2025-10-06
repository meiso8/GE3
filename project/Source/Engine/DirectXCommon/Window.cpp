#include "Window.h"
#include"ImGuiClass.h"

//ComPtr(コムポインタ)
#include<wrl.h>
#include<cassert>

#include<d3d12.h>
#include<dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//libのリンクはヘッダに書いてはいけない
//任意のひとつのcppに記述するかプロジェクトの設定で行う
//libのリンク includeのすぐ後ろに書くとよい

// ウィンドウプロシージャ
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

#ifdef _DEBUG

    //ImGuiにメッセージを渡す
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }

#endif

    //メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        //ウィンドウが破棄された
    case WM_DESTROY:
        //OSに対して、アプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    //標準メッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


void Window::Create(const std::wstring& title, const int32_t& clientWidth, const int32_t& clientHeight) {

    clientWidth_ = clientWidth;
    clientHeight_ = clientHeight;

    //main関数の先頭でComの初期化を行う
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));

#pragma region ウィンドウクラスの登録

    //ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    //ウィンドウクラス名
    wc_.lpszClassName = L"CG2WindowClass";
    //インスタンスハンドル
    wc_.hInstance = GetModuleHandle(nullptr);
    //カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //ウィンドウクラス名を登録する
    RegisterClass(&wc_);

#pragma endregion

#pragma region ウィンドウサイズの設定

    //ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0,0,clientWidth_,clientHeight_ };

    //クライアント領域をもとに実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

#pragma endregion

#pragma region ウィンドウの生成と表示

    //ウィンドウの生成
    //CreateWindowの戻り値であるHWNDはウィンドウハンドルを呼びウィンドウを表す識別子である
    hwnd_ = CreateWindow(
        wc_.lpszClassName,         // 利用するクラス名
        title.c_str(),                   // タイトルバーの文字(何でもいい)
        WS_OVERLAPPEDWINDOW,      // よく見るウィンドウスタイル
        CW_USEDEFAULT,            // 表示X座標(Windowsに任せる)
        CW_USEDEFAULT,            // 表示Y座標(WindowsOsに任せる)
        wrc.right - wrc.left,     // ウィンドウ横幅
        wrc.bottom - wrc.top,      // ウィンドウ縦幅
        nullptr,                  // 親ウィンドウハンドル
        nullptr,                  // メニューハンドル
        wc_.hInstance,             // インスタンスハンドル
        nullptr);                 // オプション



    //ウィンドウを表示する
    ShowWindow(hwnd_, SW_SHOW);

#pragma endregion 


}

bool Window::ProcessMassage()
{
    MSG msg{};
    //Windowにメッセージが来ていたら最優先で処理させる
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //メッセージが来たら終了する
    if (msg.message == WM_QUIT) {
        return true;
    }

    return false;
}

void Window::Finalize()
{

    CloseWindow(hwnd_);
    CoUninitialize();
}

