#pragma once
#include"ButtobiEngine.h"
#include<map>
#include"SceneManager.h"
class Game:public ButtobiEngine
{
public:
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;
    void Debug()override;
private:

    std::map<const std::string, std::unique_ptr<BaseScene>> scenes;
};

