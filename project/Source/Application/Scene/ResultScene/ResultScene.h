#pragma once

#include"Sprite.h"
#include<memory>
#include"BaseScene.h"

class ResultScene :public BaseScene
{
public:
    ResultScene();
    ~ResultScene() override;
    void Initialize()override;
    void Update()override;
    void DrawModel()override;
    void DrawSprite()override;
   void SceneChangeUpdate()override;
private:
    bool isSkipDraw_ = false;
    float timer_ = 0.0f;
    std::unique_ptr<Sprite> creditSprite_;
    float scrollSpeed_ = 0.001f; // スクロール速度
    std::unique_ptr<Sprite> skipSprite_;
    std::unique_ptr<Sprite> symbolSprite_; 
    std::vector<TextureFactory::Handle> textureSequence_;
    size_t currentIndex_ = 0;
    float switchInterval_ = 5.0f; // 1秒ごとに切り替え

    float alpha_ = 1.0f;
    float fadeSpeed_ = 0.05f; // フェードイン速度（調整可）
    float lookTimer_ = 0.0f;
};
