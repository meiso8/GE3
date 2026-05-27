#include"ModelFactory.h"
#include"ModelManager.h"
void ModelFactory::Load() {

    //モデルのロード

    ModelManager::LoadModel("Resource/Models/playerGirl", "playerGirl.gltf");



    ModelManager::LoadModel("Resource/Models/Box", "Box.obj");
    ModelManager::LoadModel("Resource/Models/AmenRa", "AmenRa.obj");
    ModelManager::LoadModel("Resource/Models/Medjed", "medjed.gltf");
    ModelManager::LoadModel("Resource/Models/player", "player.obj");
    ModelManager::LoadModel("Resource/Models/people", "people.obj");
    ModelManager::LoadModel("Resource/Models/mummy", "mummy.obj");
    ModelManager::LoadModel("Resource/Models/mummy", "dummyMummy.gltf");
    ModelManager::LoadModel("Resource/Models/mummyRoom", "mummyRoom.obj");
    ModelManager::LoadModel("Resource/Models/mummy", "coffin.gltf");

    ModelManager::LoadModel("Resource/Models/hart", "hart.obj");
    ModelManager::LoadModel("Resource/Models/sunMedal", "sunMedal.obj");
    ModelManager::LoadModel("Resource/Models/sunMedal", "crowbarItem.obj");

    ModelManager::LoadModel("Resource/Models/Papyrus", "Papyrus.obj");
    ModelManager::LoadModel("Resource/Models/Water", "Water.obj");

};