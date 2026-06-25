#pragma once  
#include <Windows.h>  
#include<cstdint>//int32_tを使うため
#include<string>

class Window
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public:
    /// @brief Windowクラスのコンストラクタ
    /// @param title ウィンドウタイトル
    /// @param clientWidth ウィンドウの幅
    /// @param clientHeight ウィンドウの高さ
    Window(const std::wstring& title, const int32_t& clientWidth, const int32_t& clientHeight);
    /// @brief 終了処理
    void Finalize();

    HINSTANCE GetHInstance()const { return wc_.hInstance; }
    HWND GetHwnd() { return hwnd_; }

    static int32_t GetClientWidth() {
        return clientWidth_;
    }
    static int32_t GetClientHeight() {
        return clientHeight_;
    }
    //メッセージを受け取る処理
    static bool ProcessMassage();
private:
    /// @brief ウィンドウクラス  
    WNDCLASS wc_{};
    /// @brief ウィンドウハンドル  
   HWND hwnd_ = nullptr;
    //クライアント領域のサイズ  
   static int32_t clientWidth_;
    static int32_t clientHeight_;
};
