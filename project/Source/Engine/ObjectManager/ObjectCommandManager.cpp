#include "ObjectCommandManager.h"
#include"ObjectManager.h"

void ObjectCommandManager::Initialize()
{
    //戦闘を示すためのコマンドを挿入
    icommands_.emplace_back(std::make_shared<EmptyCommand>());
    //イテレータも戦闘を示す
    commItr_ = icommands_.begin();
}

void ObjectCommandManager::Do(std::shared_ptr<ObjectCommandBase> spCommand)
{
    //コマンドの処理を実行
    spCommand->Do();

    //イテレーターが最後尾でなければUndo済みコマンドを削除する
    if (commItr_ != std::prev(icommands_.end())) {
        //イテレーターより後ろのコマンドをクリアする
        icommands_.erase(std::next(commItr_), icommands_.end());
    }

    //コマンドを追加
    icommands_.emplace_back(spCommand);

    //一番最後のコマンドのイテレータを取得
    commItr_ = std::prev(icommands_.end());

}

void ObjectCommandManager::UnDo()
{

    //既にイテレータが戦闘だったら何もしない
    if (commItr_ == icommands_.begin()) {
        return;
    }

    //今イテレータが指しているコマンドのUndo関数を呼ぶ
    std::shared_ptr<ObjectCommandBase> spCommand = *commItr_;

    if (spCommand == nullptr) {
        return;
    }

    spCommand->UnDo();

    //イテレータを一つ前に戻す
    commItr_ = std::prev(commItr_);
}

void ObjectCommandManager::ReDo()
{
    //既にイテレータが最後の要素を示していたら何もしない
    if (commItr_ == std::prev(icommands_.end())) {
        return;
    }

    //イテレータを一つ後ろに進める
    ++commItr_;
    //イテレータが示しているコマンドのDo関数を呼ぶ
    commItr_->get()->Do();

}


void UpdateGuizmoCommand::Do()
{
    // IDからオブジェクトを検索して、操作後の状態を適用する
    auto* obj = ObjectManager::GetInstance()->FindObjectByID(objectID_);
    if (obj) {
        obj->SetTransform(after_);
    }
}

void UpdateGuizmoCommand::UnDo()
{// IDからオブジェクトを検索して、操作前の状態に戻す

    auto* obj = ObjectManager::GetInstance()->FindObjectByID(objectID_);
    if (obj) {
        obj->SetTransform(before_);

    }
}
