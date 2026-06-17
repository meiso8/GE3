#pragma once

#include <Windows.h>  

//ComPtr(コムポインタ)
#include<wrl.h>
#include<dxgi1_6.h>
#include<d3d12.h>

#ifdef USE_IMGUI
#include"../externals/imgui/imgui.h"
#include"../externals/imgui/imgui_impl_dx12.h"
#include"../externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include"Window.h"
#include"SwapChain.h"
#include"RenderTargetView.h"

class Object3d;
class Camera;

class CommandList;

class ImGuiClass
{
public:
    void Initialize(Window& window,
        const Microsoft::WRL::ComPtr<ID3D12Device>& device,
        SwapChain& swapChain,
        RenderTargetView& rtv);

    void FrameStart();
    void Render();
    void DrawImGui(ID3D12GraphicsCommandList* commandList);
    void ShutDown();
   static  void UpdateGuizmo(Camera& camera, Object3d& targetObject);
   void DrawModelLoaderWindow();
   static void DropFiles(WPARAM wParam);
};


#endif