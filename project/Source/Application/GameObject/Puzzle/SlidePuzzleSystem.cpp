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

void SlidePuzzleSystem::Finalize()
{
    if (puzzle_) {
        puzzle_.reset();
    }
 
}

void SlidePuzzleSystem::Initialize()
{

    clearTimer_ = maxTimer_;
    isActive_ = false;
    isEnd_ = false;
    puzzle_->Init();
    puzzleObj_->Initialize();
}

void SlidePuzzleSystem::Update(const Vector2& screenPos)
{
    //クリアしたら
    if (puzzle_->IsClear()) {

        if (clearTimer_ == maxTimer_) {
            SoundManager::PlayCorrectSE();
        }

        clearTimer_ -= TimeManager::DeltaTime();
        clearTimer_ = std::clamp(clearTimer_, 0.0f, maxTimer_);

        if (clearTimer_ == 0.0f) {
            //開ける
            puzzleObj_->SetIsOpen(true);
            isEnd_ = true;
            isActive_ = false;
        }
    }

    if (isActive_) {
  
        puzzle_->Game(screenPos);

    }

    puzzleObj_->Update();

    //デバック用に一旦
    if (Input::IsTriggerKey(DIK_P)) {
        isEnd_ = true;
    }

#ifdef USE_IMGUI
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
    AABB box = GetAABBWorldPos(puzzleObj_.get()); // AABBなど
    Vector3 itemPos = puzzleObj_->GetWorldPosition();

    if (ray.IntersectsAABB(box, itemPos)) {

        puzzleObj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });

        if (InputBind::IsClick()) {
            if (!isActive_) {
                isActive_ = true;
            }

        }
    }

}


