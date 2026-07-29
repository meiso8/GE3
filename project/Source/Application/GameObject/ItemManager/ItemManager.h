#pragma once
#include "ItemSlot/ItemSlot.h"

class RaySprite;

class ItemManager {
public:
    /// @brief 太陽円盤取得フラグの取得静的関数
    /// @return 太陽円盤取得フラグ
    static bool IsGetSolarDisc() { return isGetSolarDisc_; }
    /// @brief 太陽円盤取得フラグのセット
    /// @param flag フラグをセット
    static void SetIsGetSolarDisc(const bool flag) { isGetSolarDisc_ = flag; }
    ~ItemManager();
    /// @brief コンストラクタ
    ItemManager();
    /// @brief 初期化
    void Init();
    /// @brief 更新
    void Update();
    /// @brief アイテムの描画
    void DrawGetItem();
    /// @brief 描画関数
    /// @param camera カメラ
    void Draw(Camera& camera);
    /// @brief UIの描画
    void DrawUI();
    /// @brief アイテムがあるかどうかのフラグ取得関数
    /// @param name アイテム名
    /// @return アイテムがあるかどうかのフラグ
    bool HasItem(const std::string& name);
    /// @brief アイテム取得関数
    /// @param name アイテム名
    /// @return アイテム
    std::shared_ptr<Item> GetItem(const std::string& name);
    /// @brief 指定アイテムをスロットに追加する関数
    /// @param name アイテム名
    /// @return アイテムスロットに入ったか
    bool AddItemToSlot(const std::string& name);
    /// @brief アイテムの生成
    /// @param itemNames アイテム名を入れる
    void GenerateItems(const std::vector<std::string>& itemNames);
    /// @brief アイテムスロット取得関数
    /// @return アイテムスロット
    ItemSlot& GetItemSlot() { return itemSlot_; }
    /// @brief アイテムがレイにヒットしているかを判定する関数
    /// @param raySprite レイスプライトクラスを入れる
    /// @return レイにヒットしているアイテムを返す
    std::shared_ptr<Item> RaycastHitItem(RaySprite& raySprite);
    /// @brief アイテムスロットからアイテムを使う関数
    /// @param pos 置きたい大体の位置
    /// @param name アイテム名
    void UseItemFromSlot(const Vector3& pos, const char* name);
    /// @brief プレイヤーの手の行列をセットする
    /// @param matrix 行列
    void SetPlayerHandMatrix(Matrix4x4* matrix) { playerHandMatrix_ = matrix; }
private:
    /// @brief アイテムの辞書登録
    std::map<std::string, std::shared_ptr<Item>> items_;
    //アイテムスロットクラス
    ItemSlot itemSlot_;
    /// @brief 太陽円盤取得フラグの静的メンバ変数
    static bool isGetSolarDisc_;
    //プレイヤーの手の行列
    Matrix4x4* playerHandMatrix_ = nullptr;
};
