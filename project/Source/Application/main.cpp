#include<numbers>
#include"MyEngine.h"
#include"Player.h"
#include"Particle/Particle.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    MyEngine* myEngine = MyEngine::GetInstance();
    myEngine->Create(L"2102", WIN_WIDTH, WIN_HEIGHT);
    //DirectX初期化処理の末尾に追加する
    //音声クラスの作成
    Sound sound;

    srand(static_cast<unsigned int>(time(nullptr)));

#pragma region//XAudio全体の初期化と音声の読み込み

    //音声読み込み SoundDataの変数を増やせばメモリが許す限りいくつでも読み込める。
    SoundData bgmData[2] = {
        sound.SoundLoad(L"resources/Sounds/dreamcore.mp3"),
        sound.SoundLoad(L"resources/Sounds/kiritan.mp3") };

#pragma endregion

    bool isDebug = false;

    DebugUI debugUI;

#pragma region//Camera

    Camera camera;
    Transform cameraTransform{ { 1.0f, 1.0f, 1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-10.0f } };
    camera.SetTransform(cameraTransform);
    camera.Initialize(static_cast<float>(WIN_WIDTH), static_cast<float>(WIN_HEIGHT), false);


    Camera cameraSprite;
    cameraSprite.Initialize(static_cast<float>(WIN_WIDTH), static_cast<float>(WIN_HEIGHT), true);

#pragma endregion

    Texture textures(myEngine->GetDevice(), myEngine->GetCommandList());
    textures.Load("resources/white1x1.png");

    Texture textures2(myEngine->GetDevice(), myEngine->GetCommandList());
    textures2.Load("resources/numbers.png");

    Texture textures3(myEngine->GetDevice(), myEngine->GetCommandList());
    textures3.Load("resources/uvChecker.png");

    //ShaderResourceViewを作る
    ShaderResourceView srv[3] = {};

    srv[0].Create(textures, 1, myEngine->GetDevice(), myEngine->GetSrvDescriptorHeap());
    srv[1].Create(textures2, 2, myEngine->GetDevice(), myEngine->GetSrvDescriptorHeap());
    srv[2].Create(textures3, 3, myEngine->GetDevice(), myEngine->GetSrvDescriptorHeap());


    DrawGrid grid = DrawGrid(myEngine->GetDevice(), myEngine->GetModelConfig(), myEngine->GetPSO(0));

    Sprite sprite;
    sprite.Create(myEngine->GetDevice(), myEngine->GetModelConfig());
    sprite.SetSize(Vector2(256.0f, 256.0f));
    //sprite.SetTranslate({ WIN_WIDTH - sprite.GetSize().x,WIN_HEIGHT - sprite.GetSize().y,0.0f });
    sprite.SetTranslate({ 0.0f,0.0f,0.0f });

    const ModelData modelData = LoadObjeFile("resources/player", "player.obj");
    std::unique_ptr<Player>player;
    player = std::make_unique<Player>(modelData);
    player.get()->Init();

    Cube cube[2];
    cube[0].Create(myEngine->GetDevice(), myEngine->GetModelConfig());
    cube[1].Create(myEngine->GetDevice(), myEngine->GetModelConfig());
    WorldTransform cubeWorldTransform;
    cubeWorldTransform.Initialize();
    cubeWorldTransform.SetRotationY(std::numbers::pi_v<float> / 4.0f);
    WorldTransformUpdate(cubeWorldTransform);

    Vector4 worldColor = { 0.6f,0.6f,0.6f,1.0f };

    //MSG msg{};

    int32_t lightType = MaterialResource::LIGHTTYPE::NONE;
    uint32_t blendMode = BlendMode::kBlendModeNone;

    Particle particle;
    particle.Create(myEngine->GetDevice(),myEngine->GetCommandList(),myEngine->GetRootSignature());

    // =============================================
    //ウィンドウのxボタンが押されるまでループ メインループ
    // =============================================
    while (true) {

        //メッセージが来たらループを抜ける
        if (myEngine->GetWC().ProcessMassage()) {
            break;
        }

        //エンジンのアップデート
        myEngine->Update();

#pragma region //ゲームの処理


#ifdef _DEBUG

        if (Input::GetInstance()->IsTriggerKey(DIK_SPACE)) {
            //デバッグの切り替え
            isDebug = (isDebug) ? false : true;
        }

        if (Input::GetInstance()->IsTriggerKey(DIK_O)) {
            camera.SetOrthographic(true);
        }


        if (isDebug) {
            debugUI.CheckDirectionalLight(myEngine->GetDirectionalLightData(), lightType);
            debugUI.CheckFPS();

            debugUI.CheckInput(*Input::GetInstance());
            /*        debugUI.CheckColor(worldColor);*/
            debugUI.CheckCamera(camera);
            debugUI.CheckBlendMode(blendMode);

            //デバッグカメラに切り替え
                   //視点操作
            Input::GetInstance()->EyeOperation(camera);

        }

        camera.Update();

        player.get()->Update();

        cube[0].SetColor({ 1.0f, 1.0f, 1.0f, 0.2f
            });

        cube[1].SetColor({ 1.0f, 1.0f, 1.0f, 0.2f
            });

#endif

#pragma region //描画

        myEngine->PreCommandSet(worldColor);

        sprite.PreDraw(myEngine->GetPSO(kBlendModeNormal));
        sprite.Draw(srv[2], cameraSprite, lightType);

        //グリッドの描画
        if (isDebug) {
            grid.Draw(srv[0], camera);
        }

        cube[0].PreDraw(myEngine->GetPSO(kBlendModeNormal));
        cube[0].Draw(srv[1], camera, MakeIdentity4x4(), lightType);
        cube[1].Draw(srv[1], camera, cubeWorldTransform.matWorld_, lightType);

        myEngine->SetBlendMode(blendMode);
        player.get()->Draw(camera, lightType);
        myEngine->SetBlendMode();

        particle.Draw(camera,srv[2]);

        myEngine->PostCommandSet();

#pragma endregion

    }


    myEngine->End();

    return 0;
}
