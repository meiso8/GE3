#include "BackGround.h"

BackGround::BackGround()
{
    world_ = std::make_unique<World>();
    field_ = std::make_unique<Field>();
    building_ = std::make_unique<Building>();

}

void BackGround::Initialize()
{
    world_->Init();
    field_->Init();
    building_->Init();
}

void BackGround::Update()
{
    building_->Update();
    world_->Update();
}

void BackGround::Draw()
{
    world_->Draw();

    building_->Draw();
}

void BackGround::DrawField()
{
    field_->Draw();
}

void BackGround::UpdateApperMedjed()
{
    world_->UpdateColor();
    field_->Update();
}
