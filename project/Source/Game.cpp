#include "Game.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

void Game::Initialize()
{
    
    //エンジンの生成
    ButtobiEngine::Create(L"LE3A_19_ヨシダ_トモカ_打ち倒すもの", WIN_WIDTH, WIN_HEIGHT);
}

void Game::Finalize()
{
    // エンジンの終了
    ButtobiEngine::Finalize();
}

void Game::Update()
{
    // エンジンの更新処理
    ButtobiEngine::Update();
}

void Game::Draw()
{
    // エンジンの描画前処理
    ButtobiEngine::PreCommandSet();

    // エンジンの描画後処理
    ButtobiEngine::PostCommandSet();
}

void Game::Debug()
{

#ifdef USE_IMGUI
    // デバック用
    ButtobiEngine::Debug();
#endif // USE_IMGUI

}
