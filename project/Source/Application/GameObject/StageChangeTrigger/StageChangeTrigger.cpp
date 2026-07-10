#include "StageChangeTrigger.h"
#include"../../StageManager/StageManager.h"
#include"Log.h"
#include"PrimitiveFactory/PrimitiveFactory.h"
#include"ModelManager.h"
#include"Model.h"
#include"DebugUI.h"
#include"ObjectManager/ObjectManager.h"

StageChangeTrigger::StageChangeTrigger()
{
  
    object_ = std::make_unique<Object3d>();
    object_->Create();

    // 穴の大きさに合わせてAABBを設定 (例: 1m × 1m × 1m の立方体)
    SetAABB({ .min = { -0.5f, -0.5f, -0.5f }, .max = { 0.5f, 0.5f, 0.5f } });
    // 自身の属性を設定
    SetCollisionAttribute(CollisionTag::GetTag("StageTrigger"));
    // プレイヤーに設定
    SetCollisionMask(CollisionTag::GetTag("Player"));
    //ワールド行列をセットする
    SetWorldMatrix(*object_);
}

StageChangeTrigger::~StageChangeTrigger()
{

}

void StageChangeTrigger::Create(
    const LevelData::StageChangeTriggerData& data
)
{
    //オブジェクトの初期化
    object_->Initialize();
    //オブジェクト名の設定
    object_->SetObjectType("StageChangeTrigger");
    //温度の設定
    object_->SetTemperature(data.materialData.tempareture);
    //カラーの設定
    object_->SetColor(data.materialData.color);
    //輝度の設定
    object_->SetShininess(data.materialData.shininess);
    //環境光
    object_->SetEnvironmentCoefficient(data.materialData.environmentCoefficient);
    //メッシュの設定
    if (data.filePath.fileName != "empty") {
        //メッシュ名が空ではないとき
        if (PrimitiveFactory::GetPrimitives().contains(data.filePath.fileName)) {
            //プリミティブだったら
            object_->SetMeshAndMaterial(PrimitiveFactory::GetPrimitiveForName(data.filePath.fileName));
        } else {
            object_->SetMeshAndMaterial(ModelManager::LoadModelAndGet(data.filePath.directoryPath  + "/" + data.filePath.fileName));
            object_->SetTextureHandle(static_cast<TextureFactory::Handle>(data.materialData.textureHandle));
        }
    }

    //ステージ名の設定
    object_ -> SetNextStageName(data.nextStageName);

    //トランスフォームのセット
    object_->SetTransform(data.transform);

    //コライダーの中心を記録
    SetCenter(data.colliderData.center);
    
    Vector3 halfSize = data.colliderData.size * 0.5f; // AABBは中心からの「半径（半分のサイズ）」を指定するため
    //AABBの大きさを記録
    SetAABB({ .min = -halfSize, .max = halfSize });
    //レジスターに入れる
    object_->RegisterObject();
}

void StageChangeTrigger::Update()
{
    object_->Update();

}

void StageChangeTrigger::Draw(Camera& camera)
{
    object_->Draw(camera);
}

void StageChangeTrigger::OnCollision(Collider* collider)
{

    // 衝突相手がプレイヤーかチェック
    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {
        //次のステージに進む
        StageManager::GetInstance()->SetNestStage(object_->GetNextStageName());
        std::string name = "GO to" + object_->GetNextStageName() + "Stage";
        LogFile::Log(name);
    }
}
