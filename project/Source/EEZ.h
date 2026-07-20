#pragma once
#include<stdint.h>
#include<memory>
#include"Transform.h"
#include<string>

class Camera;
class Body;
class Picture;
class Game;
class Food;
class Stool;
class Model;

class Human {
private:
    uint32_t age_ = 0;
    std::string name_ = "";
    std::unique_ptr<Body>body_ = nullptr;
public:
    /// @brief コンストラクタ　生まれた
    /// @param name 名前
    /// @param model 遺伝子や物質としての体
    Human(const std::string& name, std::unique_ptr<Body>body)
        :name_(name), body_(std::move(body)) {
    };
    /// @brief デストラクタ　つまり死亡すること
    virtual ~Human();
    /// @brief 初期化
    virtual void Initialize();
    /// @brief 更新処理
    virtual void Update();
    /// @brief 描画処理
    /// @param camera Camera
    virtual void Draw(Camera* camera);
    /// @brief 食べる
    /// @param food 食べ物
    /// @return　食べたら出てくるものです
    Stool Eat(std::unique_ptr<Food>food);
private:
    /// @brief 寝る
    void Sleep();
};

class TomokaYoshida:public Human {
public:
    /// @brief 絵を描く
    /// @return 絵を返す
    Picture DrawPicture();
    /// @brief ゲームを作る
    /// @return ゲームを返す
    Game CreateGame();
    /// @brief モデリング
    /// @return モデルを返す
    Model Modelling();
    /// @brief 教える
    /// @param money お金
    void Teach(long long int money);
};

