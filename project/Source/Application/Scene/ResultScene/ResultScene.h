#pragma once

#include<memory>
#include"BaseScene.h"
class Sprite;
class ResultMedjed;
class SkyboxObject3d;
class Player;
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
    std::unique_ptr<Sprite> creditSprite_ = nullptr;
    float scrollSpeed_ = 0.001f; // スクロール速度
    std::unique_ptr<Sprite> skipSprite_ = nullptr;
    std::unique_ptr<Sprite> symbolSprite_ = nullptr; 
    std::vector<TextureFactory::Handle> textureSequence_;
    //メジェド様
    std::unique_ptr<ResultMedjed>medjed_ = nullptr;
    //スカイボックス
    std::unique_ptr<SkyboxObject3d> skyBoxObj_ = nullptr;
    //プレイやー
    std::unique_ptr<Player> player_ = nullptr;


    size_t currentIndex_ = 0;
    float switchInterval_ = 5.0f; // 1秒ごとに切り替え

    float alpha_ = 1.0f;
    float fadeSpeed_ = 0.05f; // フェードイン速度（調整可）
    float lookTimer_ = 0.0f;
};
