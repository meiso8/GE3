#include<numbers>
#include"MyEngine.h"
#include"Player.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    MyEngine myEngine;
    myEngine.Create(L"2102", WIN_WIDTH, WIN_HEIGHT);
    //DirectX初期化処理の末尾に追加する
//音声クラスの作成
    Sound sound;

    FPSCounter fpsCounter;

    Input* input = Input::GetInstance();
    //入力
    input->Initialize(myEngine.GetWC(), fpsCounter.GetFPS());

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

    Texture textures(myEngine.GetDevice(), myEngine.GetCommandList());
    textures.Load("resources/sky.png");

    //ShaderResourceViewを作る
    ShaderResourceView srv = {};

    srv.Create(textures, 1, myEngine.GetDevice(), myEngine.GetSrvDescriptorHeap());

    DrawGrid grid = DrawGrid(myEngine.GetDevice(), myEngine.GetModelConfig(), myEngine.GetPSO(0));

    Sprite sprite;
    sprite.Create(myEngine.GetDevice(), myEngine.GetModelConfig());
    sprite.SetSize(Vector2(1.0f, 1.0f));
    //sprite.SetTranslate({ WIN_WIDTH - sprite.GetSize().x,WIN_HEIGHT - sprite.GetSize().y,0.0f });
    sprite.SetTranslate({ 0.0f,0.0f,0.0f });

    const ModelData modelData = LoadObjeFile("resources/player", "player.obj");
    std::unique_ptr<Player>player;
    player = std::make_unique<Player>(myEngine, modelData);
    player.get()->Init();

    Cube cube;
    cube.Create(myEngine.GetDevice(), myEngine.GetModelConfig());

    Vector4 worldColor = { 0.6f,0.6f,0.6f,1.0f };

    MSG msg{};

    int32_t lightType = MaterialResource::LIGHTTYPE::NONE;
    uint32_t blendMode = BlendMode::kBlendModeNone;

    // =============================================
    //ウィンドウのxボタンが押されるまでループ メインループ
    // =============================================
    while (msg.message != WM_QUIT) {

        //Windowにメッセージが来ていたら最優先で処理させる
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {

            //キーボード情報の取得開始
            input->Update();

            //エンジンのアップデート
            myEngine.Update();

            fpsCounter.Update();

#pragma region //ゲームの処理


#ifdef _DEBUG

            if (input->IsTriggerKey(DIK_SPACE)) {
                //デバッグの切り替え
                isDebug = (isDebug) ? false : true;
            }

            if (input->IsTriggerKey(DIK_O)) {
                camera.SetOrthographic(true);
            }


            if (isDebug) {
                debugUI.CheckDirectionalLight(myEngine.GetDirectionalLightData(), lightType);
                debugUI.CheckFPS();

                debugUI.CheckInput(*input);
                debugUI.CheckColor(worldColor);
                debugUI.CheckCamera(camera);
                debugUI.CheckBlendMode(blendMode);

                //デバッグカメラに切り替え
                       //視点操作
                input->EyeOperation(camera);

            }

            camera.Update();

            player.get()->Update();

#endif

#pragma region //描画

            myEngine.PreCommandSet(worldColor);

            sprite.PreDraw(myEngine.GetPSO(1));
            sprite.Draw(srv, camera, lightType);

            //グリッドの描画
            if (isDebug) {
                grid.Draw(srv, camera);
            }

            cube.PreDraw(myEngine.GetPSO(1));
            cube.Draw(srv, camera, MakeIdentity4x4(), lightType);

            myEngine.SetBlendMode(blendMode);
            player.get()->Draw(camera, lightType);
            myEngine.SetBlendMode();


            myEngine.PostCommandSet();

#pragma endregion

        }
    }

    myEngine.End();

    return 0;
}
