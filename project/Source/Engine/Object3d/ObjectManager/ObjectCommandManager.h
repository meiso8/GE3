#pragma once
#include<list>
#include <memory>
#include"ObjectCommandBase.h"
#include"WorldTransform.h"

//リスト戦闘でNullとして扱うためのコマンド
class UpdateGuizmoCommand :public ObjectCommandBase {
public:
    // コンストラクタで「対象のID」「操作前」「操作後」を受け取る
    UpdateGuizmoCommand(uint32_t objectID, const EulerTransform& before, const EulerTransform& after)
        : objectID_(objectID), before_(before), after_(after) {
    }
public:
    void Do()override;
    void UnDo()override;
    uint32_t objectID_;
    EulerTransform before_;
    EulerTransform after_;
};


class ObjectCommandManager
{
private:
    //リスト戦闘でNullとして扱うためのコマンド
    class EmptyCommand :public ObjectCommandBase {
    public:
        void Do()override {};
        void UnDo()override {};
    };


public:
    //初期化処理
    void Initialize();
    //コマンドを実行する関数
    void Do(std::shared_ptr<ObjectCommandBase> spCommand);

    template<typename CommType, typename...ArgTypes>
    void Do(ArgTypes...args) {
        //コマンドのインスタンスの作成と初期化
        std::shared_ptr<CommType> spNewcom = std::make_shared<CommType>(args...);
        //コマンドの実行
        Do(spNewcom);
    
    };
    //実行処理
    void UnDo();
    //再実行処理
    void ReDo();

private:
    //実行したコマンドを格納するコンテナ
    std::list<std::shared_ptr<ObjectCommandBase>> icommands_;
    //コマンドのイテレータ
    std::list<std::shared_ptr<ObjectCommandBase>>::iterator commItr_;

};

