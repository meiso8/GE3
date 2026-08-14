#pragma once
#include"Collider.h"
#include"Object3d.h"
#include<memory>

class Item :public Collider {
public:
    Item();
    virtual ~Item() = default;
    void SetModel(const std::string& tagName);

    // 使用時の効果（プレイヤーや環境に作用）
    virtual void Use() = 0;
    // 他のアイテムと組み合わせたときの処理 

    virtual std::shared_ptr<Item> CombineWith(const std::shared_ptr<Item>& other) {
        return nullptr; // デフォルトは組み合わせ不可 
    }

    virtual void Init();
    virtual void Update();
    virtual void Draw(Camera& camera);
    void OnCollision(Collider* collider)override;

    //スクリーン座標に変換する
    virtual void LerpScreenPosAndScale(const Vector2& screenPos, const Matrix4x4& matInverseVPV);
    //スロットアイテムとして描画する
    void DrawForSlotItem(Camera& camera);


    Vector3 GetWorldPosition() const {
        return object_->GetWorldTransform().GetWorldPosition();
    };

    void Rotate();
    void SetRotate(const Vector3& rotate) { object_->SetRotate(rotate); };
    void SetScale(const Vector3& scale) { object_->SetScale(scale); }

    void InitScale() const { object_->SetScale(Math::UNIT_SCALE); };

    void SetScreenStartPos();

    void SetStartEndPos(const Vector3& start, const Vector3& end);

    const Vector3& GetUseRotate() { return useRotate_; }


    bool IsUsed() { return isUsed_; };
    bool IsGet() { return isGet_; };
    bool IsGetAnimEnd() { return isAnimEnd_; }

    void SetIsGet(const bool flag) { isGet_ = flag; }
    const float GetAnimTimer() { return aniTimer_; }
    std::shared_ptr<Object3d>& GetObject3d() { return object_; };
    const std::string& GetName() {return name_; };
    void SetParentMat(Matrix4x4* parent) { parent_ = parent; }
    void UpdateAniTimer(const float& endTime = 4.0f);
    bool GetMeltEnd() { return isMeltEnd_; };
protected:

    float aniTimer_ = 0.0f;

    bool isUsed_ = false;
    //取得する
    bool isGet_ = false;
    bool isAnimEnd_ = false;
    //溶け終わり
    bool isMeltEnd_ =  false;

    // 3Dオブジェクトの参照（描画や当たり判定用）
    std::shared_ptr<Object3d> object_ = nullptr;
    std::unique_ptr<Object3d>handItemObj_ = nullptr;
    // アイテムの名前や説明 
    std::string name_;
    std::string description_;

    Vector3 useRotate_ = { 4.7f,1.57f,0.0f };

    Vector3 startPos_ = { 0.0f };
    Vector3 endPos_ = { 0.0f };
 
    Matrix4x4* parent_ = nullptr;

    float screenEndSize_ = 0.03125f;
};
