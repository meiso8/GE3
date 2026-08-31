#pragma once
#include"Building/Building.h"

class PapyrusWall:public Building
{
public:
    PapyrusWall();
    void Draw()override;
private:
    void SetWallAABB()override;
    void SetWallPos()override;

};

