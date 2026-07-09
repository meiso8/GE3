#pragma once
#include"Collider.h"
#include"Object3d.h"
#include<memory>

class Item :public Collider {
public:
    Item();
    virtual ~Item() = default;
    virtual void SetModel(const std::string& tagName);

    // 使用時の効果（プレイヤーや環境に作用）
    virtual void Use() = 0;
    // 他のアイテムと組み合わせたときの処理 

    virtual std::shared_ptr<Item> CombineWith(const std::shared_ptr<Item>& other) {
        return nullptr; // デフォルトは組み合わせ不可 
    }

    // UI表示用（ImGuiなど） 
    virtual void Init();
    virtual void DrawInfoUI();
    virtual void Update();
    virtual void Draw(Camera& camera);
    void DrawForSlotItem(Camera& camera);
    void OnCollision(Collider* collider)override;

    Vector3 GetWorldPosition() const {
        return object_->GetWorldTransform().GetWorldPosition();
    };

    void Rotate();
    void SetRotate(const Vector3& rotate) { object_->SetRotate(rotate); };
    void Scale(const Vector3 start, const Vector3 end);
    void SetScale(const Vector3& scale) { object_->SetScale(scale); }
    void InitScale() const { object_->SetScale(Math::UNIT_SCALE); };
    void SetScreenStartPos();
    void UpdateAniTimer(const float& endTime = 4.0f);

    virtual void LerpScreenPos(const Vector2& screenPos, const Matrix4x4& matInverseVPV);
    void SetStartEndPos(const Vector3& start, const Vector3& end);

    const Vector3& GetUseRotate() { return useRotate_; }
    const Vector3& GetEndPosOffset() { return endPosOffset_; }
    const Vector3& GetStartPosOffset() { return startPosOffset_; }

    bool IsUsed() { return isUsed_; };
    bool IsGet() { return isGet_; };
    bool IsGetAnimEnd() { return isGetAnimEnd_; }
    void SetIsGet(const bool flag) { isGet_ = flag; }
    const float GetAnimTimer() { return aniTimer_; }
    std::shared_ptr<Object3d>& GetObject3d() { return object_; };
    const std::string& GetName() {return name_; };
protected:

    float aniTimer_ = 0.0f;

    bool isUsed_ = false;
    bool isGet_ = false;
    bool isGetAnimEnd_ = false;

    // 3Dオブジェクトの参照（描画や当たり判定用）
    std::shared_ptr<Object3d> object_ = nullptr;
    // アイテムの名前や説明 
    std::string name_;
    std::string description_;

    Vector3 useRotate_ = { 4.7f,1.57f,0.0f };
    Vector3 endPosOffset_ = { -0.3f,0.3f,0.01f };
    Vector3 startPosOffset_ = { -0.3f,0.5f,0.01f };

    Vector3 startPos_ = { 0.0f };
    Vector3 endPos_ = { 0.0f };
};
