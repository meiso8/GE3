#include"SceneFactory.h"
#include "FreeTypeScene/FreeTypeScene.h"
#include"GameScene/GameScene.h"
#include"TitleScene/TitleScene.h"
#include"ResultScene/ResultScene.h"
#include"SceneManager.h"

void SceneFactory::Create()
{
    SceneManager::SetMap("FreeType", std::move(std::make_unique < FreeTypeScene>()));
    SceneManager::SetMap("Title", std::move(std::make_unique < TitleScene>()));
    SceneManager::SetMap("Game", std::move(std::make_unique < GameScene>()));
    SceneManager::SetMap("Result", std::move(std::make_unique < ResultScene>()));
    SceneManager::SetNextScene("Title");
#ifdef _DEVELOP
    SceneManager::SetNextScene("FreeType");
#endif
    SceneManager::InitScene();
}
