#include"Game.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::unique_ptr<ButtobiEngine> game = std::make_unique<Game>();

    game->Run();

    return 0;
}
