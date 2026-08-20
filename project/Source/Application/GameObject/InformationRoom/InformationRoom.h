#pragma once
#include "Building/Building.h"
class InformationRoom :
    public Building
{
public:
    InformationRoom();
    void Draw(Camera& camera) override;

private:
    void SetWallPos() override;
    void SetWallAABB() override;
};

