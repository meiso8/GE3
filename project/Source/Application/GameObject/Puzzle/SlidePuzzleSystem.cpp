#include "SlidePuzzleSystem.h"
#include"SoundManager/SoundManager.h"
#include<algorithm>
#include"CollisionManager.h"
#include"InputBind.h"
#include"TimeManager.h"

bool SlidePuzzleSystem::isActive_ = false;
bool SlidePuzzleSystem::isEnd_ = false;
std::unique_ptr<Puzzle>SlidePuzzleSystem::puzzle_ = nullptr;

SlidePuzzleSystem::SlidePuzzleSystem()
{
    puzzle_ = std::make_unique<Puzzle>(3,3);
    puzzle_->SetTexture(TextureFactory::PUZZLE1);
    puzzleObj_ = std::make_unique<PuzzleObj>();
}

SlidePuzzleSystem::~SlidePuzzleSystem()
{
    puzzle_.reset();
    puzzleObj_.reset();
}


void SlidePuzzleSystem::Initialize()
{

    isActive_ = false;
    isEnd_ = false;
    puzzle_->Init();
    puzzleObj_->Initialize();
}

void SlidePuzzleSystem::Update(const Vector2& screenPos)
{
    //クリアしたら
    if (puzzle_->IsClear()) {
        if (!isEnd_) {
            SoundManager::PlayCorrectSE();
            //開ける
            puzzleObj_->SetIsOpen(true);
            isActive_ = false;
            isEnd_ = true;
      
        }
    }

    if (isActive_) {
  
        puzzle_->Game(screenPos);

    }

    puzzleObj_->Update();

#ifdef USE_IMGUI

    //デバック用に一旦
    if (Input::IsTriggerKey(DIK_P)) {
        isEnd_ = true;
    }

    ImGui::Begin("Debug");
    ImGui::Checkbox("isEndPuzzle", &isEnd_);
    ImGui::End();
#endif //USE_IMGUI

}

void SlidePuzzleSystem::Draw(Camera& camera)
{
    puzzleObj_->Draw(camera);
}

void SlidePuzzleSystem::DrawUI()
{
    if (isActive_) {
        puzzle_->Draw();
    }
}

void SlidePuzzleSystem::RayCastHit(RaySprite& ray)
{
    //アクティブならヒットさせない
    if (isActive_) {
        return;
    }

    if (ray.Intersect(puzzleObj_.get())) {

        puzzleObj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });

        if (InputBind::IsClick()) {
            if (!isActive_) {
                isActive_ = true;
            }

        }
    }

}


