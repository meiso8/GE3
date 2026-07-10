#include "Puzzle.h"
#include"Input.h"
#define rep(i,n)for(int i =0;i < n;++i)
using namespace std;
#include"DebugUI.h"
#include"Collision.h"
#include"Sound.h"
#include"SoundManager/SoundManager.h"
#include"InputBind.h"

void Puzzle::Change(int x, int y)
{
    int p1 = y * vertical_ + x;
    int p2 = -1;
    int maxIndex = maxArrayNum_ - 1;

    if (x > 0 && panel_[p1 - 1] == maxIndex) p2 = p1 - 1;
    if (x < horizontal_ - 1 && panel_[p1 + 1] == maxIndex) p2 = p1 + 1;
    if (y > 0 && panel_[p1 - horizontal_] == maxIndex) p2 = p1 - horizontal_;
    if (y < vertical_ - 1 && panel_[p1 + horizontal_] == maxIndex) p2 = p1 + horizontal_;

    if (p2 != -1) {
        //入れ替わったら行われる
        panel_[p2] = panel_[p1];
        panel_[p1] = maxIndex;
    }

}

Puzzle::Puzzle(const int horizontal, const int vertical)
{
    horizontal_ = horizontal;
    vertical_ = vertical;
    maxArrayNum_ = vertical_ * horizontal_;

    centerPos_ = { float(Window::GetClientWidth() * 0.5f),float(Window::GetClientHeight() * 0.5f) };

    sprite_ = make_unique<Sprite>();
    sprite_->Create(TextureFactory::PUZZLE, centerPos_);
    sprite_->SetAnchorPoint({ 0.5f,0.5f });

    Vector2 size = sprite_->GetSize();
    size_ = { size.x / horizontal_,size.y / vertical_ };

    centerPos_ -= size_ * static_cast<float>(horizontal_) * 0.375f;

    sprites_.resize(maxArrayNum_);
    panel_.resize(maxArrayNum_);

    rep(i, maxArrayNum_) sprites_[i] = make_unique<Sprite>(),
        sprites_[i]->Create(TextureFactory::PUZZLE1, { 0.0f,0.0f });

    for (int i = 0; i < maxArrayNum_; ++i) {
        int col = i % horizontal_; // 横方向のインデックス（0〜3）
        int row = i / horizontal_; // 縦方向のインデックス（0〜3）
        sprites_[i]->SetSize(size_);
        sprites_[i]->SetTextureSize(size_);
        sprites_[i]->SetTextureLeftTop({ col * size_.x, row * size_.y });
        sprites_[i]->SetAnchorPoint({ 0.5f,0.5f });
        sprites_[i]->SetPosition({ centerPos_.x + col * size_.x,centerPos_.y + row * size_.y });
    }

}

Puzzle::~Puzzle()
{
}

void Puzzle::Init()
{
    isClear_ = false;
    Shuffle();

}

void Puzzle::Shuffle()
{
    rep(i, maxArrayNum_) panel_[i] = i;
    rep(j, 1000) Change(rand() % horizontal_, rand() % vertical_);
}

void Puzzle::Game(const Vector2& screenPos)
{

    if (isClear_) {
        return;
    }

    int clickedIndex = -1;

    for (int i = 0; i < maxArrayNum_; ++i) {

        Sprite* sprite = sprites_[panel_[i]].get();

        if (IsCollision(screenPos, *sprite)) {
            sprite->SetColor({ 1.0f,0.0f,0.0f,1.0f });

            if (InputBind::IsClick()) {
                clickedIndex = i;
                Sound::PlaySE(SoundFactory::MOVE_ROCK,0.5f);
            }

        } else {
            sprite->SetColor({ 1.0f,1.0f,1.0f,1.0f });
        }
    }

    // クリックされたパネルがあれば処理
    if (clickedIndex != -1) {
        int x = clickedIndex % horizontal_;
        int y = clickedIndex / horizontal_;
        Change(x, y);

        // クリア判定
        isClear_ = true;
        rep(i, maxArrayNum_) if (panel_[i] != i) isClear_ = false;
    }

    for (int i = 0; i < maxArrayNum_; ++i) {
        if (panel_[i] < maxArrayNum_-1) {
            int col = i % horizontal_; // 横方向のインデックス（0〜3）
            int row = i / horizontal_; // 縦方向のインデックス（0〜3）
            sprites_[panel_[i]]->SetPosition({ centerPos_.x + col * size_.x,centerPos_.y + row * size_.y });
        }
    }

}


void Puzzle::Draw()
{
    Sprite::PreDraw();

    if (isClear_) {
        sprite_->Draw();
    } else {
        rep(i, maxArrayNum_)   if (panel_[i] < maxArrayNum_ - 1) {
            sprites_[panel_[i]]->Draw();
        }
    }

}

void Puzzle::SetTexture(const TextureFactory::Handle& handle)
{
    rep(i, maxArrayNum_)
        sprites_[i]->SetTexture(handle);
    sprite_->SetTexture(handle);
}
