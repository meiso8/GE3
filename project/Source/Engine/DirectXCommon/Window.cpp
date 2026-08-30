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
#pragma comment(lib,"winmm.lib")

#include"Input.h"
#include"Log.h"

int32_t Window::clientWidth_ = 1280;
int32_t Window::clientHeight_ = 720;
// ウィンドウプロシージャ
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

#ifdef USE_IMGUI

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
    case WM_DROPFILES: 
#ifdef USE_IMGUI
        ImGuiClass::DropFiles(wparam);
#endif
        break;
    case WM_CHAR: 
        Input::PushChar(static_cast<char32_t>(wparam)); // ← ここで文字を渡す！ 
        break;
    }

    //標準メッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
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
        LogFile::Log("Quit Window");
        return true;
    }

    return false;
}

Window::Window(const std::wstring& title, const int32_t& clientWidth, const int32_t& clientHeight)
{
    clientWidth_ = clientWidth;
    clientHeight_ = clientHeight;

    //システムタイマーの分解能を上げる
    timeBeginPeriod(1);

    //main関数の先頭でComの初期化を行う
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));

#pragma region ウィンドウクラスの登録

    //ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    //ウィンドウクラス名
    wc_.lpszClassName = L"ButtobiEngineWindowClass";
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



    // 2. ウィンドウのスタイルを設定
// WS_POPUP は枠線やタイトルバーが一切ないポップアップウィンドウを作ります
    DWORD windowStyle = WS_POPUP | WS_VISIBLE;


    //クライアント領域をもとに実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc, windowStyle, false);

#pragma endregion

#pragma region ウィンドウの生成と表示


    //ウィンドウの生成
    //CreateWindowの戻り値であるHWNDはウィンドウハンドルを呼びウィンドウを表す識別子である
    hwnd_ = CreateWindowW(
        wc_.lpszClassName,         // 利用するクラス名
        title.c_str(),                   // タイトルバーの文字(何でもいい)
        windowStyle,      // よく見るウィンドウスタイル
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

    LogFile::Log("Create WindowClass");
}

void Window::Finalize()
{
    CloseWindow(hwnd_);
    CoUninitialize();


    LogFile::Log("Finalize WindowClass");
}

