#include"SceneFactory.h"
#include "FreeTypeScene.h"
#include"SampleScene.h"
#include"TitleScene.h"
#include"ResultScene.h"

void SceneFactory::Create()
{

    SceneManager::SetMap("FreeType", std::move(std::make_unique < FreeTypeScene>()));
    SceneManager::SetMap("Title", std::move(std::make_unique < TitleScene>()));
    SceneManager::SetMap("Sample", std::move(std::make_unique < SampleScene>()));
    SceneManager::SetMap("Result", std::move(std::make_unique < ResultScene>()));
    SceneManager::SetNestScene("FreeType");


    SceneManager::InitScene();
}
