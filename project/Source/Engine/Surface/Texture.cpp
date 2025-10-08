#include"Texture.h"
#include"TextureManager.h"

std::vector<uint32_t>Texture::textureHandle_;
Texture* Texture::instance_ = nullptr;

Texture* Texture::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new Texture();
    }
    return instance_;
}

void Texture::Load()
{
    TextureManager* textureManager = TextureManager::GetInstance();
    textureHandle_.resize(TEXTURE_MAX);
    textureHandle_[WHITE_1X1] = textureManager->Load("resources/white1x1.png");

    textureHandle_[UV_CHECKER] = textureManager->Load("resources/uvChecker.png");
    textureHandle_[NUMBERS] = textureManager->Load("resources/numbers.png");
    textureHandle_[PLAYER] = textureManager->Load("resources/player.png");

}
