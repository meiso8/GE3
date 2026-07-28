#define NOMINMAX
#include "CurPos.h"
#include"Window.h"
#include"Input.h"
#include"InputBind.h"
#include<algorithm>
#include"DebugUI.h"
#include"../Puzzle/SlidePuzzleSystem.h"

CurPos::CurPos()
{
    centerPos_ = { float(Window::GetClientWidth() * 0.5f),float(Window::GetClientHeight() * 0.5f) };
    screenPos_ = centerPos_;
    //カーソルポジション
    curPos_ = std::make_unique<Sprite>();
    curPos_->Create(TextureFactory::EYE, centerPos_);
    curPos_->SetAnchorPoint({ 0.5f,0.5f });
}

void CurPos::Update()
{

    if (!Input::GetIsControllerConnected(0)) {
      
    }

#ifdef _DEBUG
    ImGui::SliderFloat2("curPos", &curPosSpeed_.x, -10.0f, 10.0f);
#endif // _DEBUG




    if (Input::IsControllerStickPosMove(BUTTON_LEFT, 0, &curPosSpeed_)) {
        screenPos_.x += curPosSpeed_.x * kSpeed_;
        screenPos_.y -= curPosSpeed_.y * kSpeed_;


    } else {

        Vector2 mouse = Input::GetMousePosFiltered();
 
        if (fabs(mouse.x) > 0.0f || fabs(mouse.y) > 0.0f) {
            screenPos_ = Input::GetCursorPosition();
        } 
        
        if (SlidePuzzleSystem::IsActive()) {

            screenPos_ = SnapCursorToNearestSprite(screenPos_, SlidePuzzleSystem::GetPuzzle()->GetSprites());

        } 
    }


    Vector2 aspect = centerPos_ * 2.0f;
    screenPos_.x = std::clamp(screenPos_.x, 0.0f, aspect.x);
    screenPos_.y = std::clamp(screenPos_.y, 0.0f, aspect.y);

    curPos_->SetPosition(screenPos_);

}


Vector2 SnapCursorToNearestSprite(const Vector2& cursorPos, std::vector<std::unique_ptr<Sprite>>& sprites, float snapThreshold) {
    float minDist = std::numeric_limits<float>::max();
    Vector2 snappedPos = cursorPos;
    
    for (auto& sprite : sprites) {
        Vector2 spritePos = sprite->GetPosition();
        float dist = Distance(cursorPos, spritePos); 
        if (dist < minDist && dist < snapThreshold) {
            minDist = dist;
            snappedPos = spritePos;
 } 
    } return snappedPos; 
}
void CurPos::Draw()
{
    curPos_->Draw();
}

void CurPos::Initialize()
{
    curPosSpeed_ = { 0.0f };
    screenPos_ = centerPos_;
    curPos_->SetPosition(screenPos_);
}
