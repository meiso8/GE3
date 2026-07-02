#pragma once
#include "Building/Building.h"

class MummyRoom : public Building
{
public:
    MummyRoom();
    void Draw(Camera& camera) override;

private:
    void SetWallPos() override;
    void SetWallAABB() override;
};
