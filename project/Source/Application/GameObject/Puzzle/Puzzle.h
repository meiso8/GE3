#pragma once
#include<vector>
#include<memory>
#include"Sprite.h"
class Puzzle
{
public:
    //水平
    int horizontal_ = 4;
    //垂直
    int vertical_ = 4;
    int maxArrayNum_ = vertical_ * horizontal_;
private:
    std::vector<int>panel_;
    //画像
    std::vector< std::unique_ptr<Sprite>>sprites_;
    //画像全て
    std::unique_ptr<Sprite>  sprite_ = nullptr;
    bool isClear_ = false;
    Vector2 size_ = { 0.0f };
    Vector2 centerPos_ = { 0.0 };
private:
    //シャッフル
    void Shuffle();
    //選択したパネルと空白を切り替える
    void Change(int x, int y);
public:
    Puzzle(const int horizontal = 4, const int vertical = 4);
    void Init();
    void Game(const Vector2& screenPos);
    void Draw();
    bool IsClear() { return isClear_; };
    void SetTexture(const TextureFactory::Handle& handle);
    //画像
    std::vector< std::unique_ptr<Sprite>>& GetSprites() { return sprites_; };
};

