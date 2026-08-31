#pragma once

#include"../Field/Field.h"
#include "../Building/Building.h"
#include"../World/World.h"
#include<memory>

class BackGround
{
private:
    std::unique_ptr<World>world_ = nullptr;
    std::unique_ptr<Field>field_ = nullptr;
    std::unique_ptr<Building>building_ = nullptr;
public:
    BackGround();
    void Initialize();
    void Update();
    void Draw();
    void DrawField();
    void UpdateApperMedjed();
    Building* GetBuilding() {
        return building_
            .get();
    };
};

