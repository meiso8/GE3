#include<numbers>
#include"MyEngine.h"
#include"Player.h"
#include"Particle/Particle.h"
#include"Lerp.h"
#include"SpriteC.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    MyEngine* myEngine = MyEngine::GetInstance();
    myEngine->Create(L"2102", WIN_WIDTH, WIN_HEIGHT);

#pragma region//XAudio全体の初期化と音声の読み込み

    //音声読み込み SoundDataの変数を増やせばメモリが許す限りいくつでも読み込める。
    SoundData bgmData[2] = {
        Sound::Load(L"resources/Sounds/dreamcore.mp3"),
        Sound::Load(L"resources/Sounds/kiritan.mp3") };

    SoundData seData[2] = {
        Sound::Load(L"resources/Sounds/poppo.mp3"),
        Sound::Load(L"resources/Sounds/broken.mp3") };

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

    Texture::GetInstance();
    Texture::Load();

    DrawGrid grid = DrawGrid(Texture::GetHandle(Texture::WHITE_1X1));

    std::vector<Sprite*>sprites;

    for (uint32_t i = 0; i < 5; ++i) {
        Sprite* sprite = new Sprite();
        if (i % 2 == 0) {
            sprite->Initialize(Texture::GetHandle(Texture::PLAYER), { 128.0f,128.0f });
        } else {
            sprite->Initialize(Texture::GetHandle(Texture::UV_CHECKER), { 128.0f,128.0f });
        }

        sprite->SetPosition({ i * 256.0f,0.0f });
        sprites.push_back(sprite);
    }

    const ModelData modelData = LoadObjeFile("resources/player", "player.obj");

    std::unique_ptr<Player>player;
    player = std::make_unique<Player>(modelData);
    player->Init();

    Cube cube[2];
    cube[0].Create(Texture::GetHandle(Texture::WHITE_1X1));
    cube[1].Create(Texture::GetHandle(Texture::WHITE_1X1));

    WorldTransform cubeWorldTransform;
    cubeWorldTransform.Initialize();
    cubeWorldTransform.SetRotationY(std::numbers::pi_v<float> / 4.0f);
    WorldTransformUpdate(cubeWorldTransform);

    Vector4 worldColor = { 0.6f,0.6f,0.6f,1.0f };

    int32_t lightType = MaterialResource::LIGHTTYPE::NONE;
    uint32_t blendMode = BlendMode::kBlendModeNone;

    Particle particle;
    particle.Create(Texture::GetHandle(Texture::UV_CHECKER));

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

        if (Input::GetInstance()->IsTriggerKey(DIK_SPACE) || Input::GetInstance()->IsJoyStickTrigger(1)) {
            //デバッグの切り替え
            isDebug = (isDebug) ? false : true;
        }

        if (Input::GetInstance()->IsTriggerKey(DIK_O)) {
            camera.SetOrthographic(true);
        }

        if (Input::GetInstance()->IsTriggerKey(DIK_A)) {
            Sound::Play(seData[0], 1.0f, false);
        }

        if (Input::GetInstance()->IsTriggerKey(DIK_S)) {
            Sound::Play(seData[1], 1.0f, false);
        }

        if (!Sound::IsPlaying()) {
            Sound::Play(bgmData[1], 0.0625f, true);
        }

        if (isDebug) {
            debugUI.CheckDirectionalLight(lightType);
            debugUI.CheckFPS();

            debugUI.CheckInput(*Input::GetInstance());
            debugUI.CheckColor(worldColor);
            debugUI.CheckCamera(camera);

            debugUI.CheckBlendMode(blendMode);
            debugUI.CheckSprite(*sprites[0]);

            //デバッグカメラに切り替え
                   //視点操作
            Input::GetInstance()->EyeOperation(camera);

        } else {

            Vector3 cameraPos = player->GetTranslate();

            Vector2 delta = { 0.0f,0.0f };
            if (Input::GetInstance()->GetJoyStickPos(&delta.x, &delta.y, Input::BUTTON_RIGHT)) {
                camera.SetRotateY(delta.x);
            }
            cameraPos.y += 1.0f + delta.y;
            cameraPos.z -= 10.0f;

            cameraPos = Lerp(cameraPos, camera.GetTranslate(), 0.01f);
            camera.SetTranslate(cameraPos);

        }

        for (Sprite* sprite : sprites) {
            sprite->UpdateUV();
        }

        camera.Update();

        player->Update();
#endif

#pragma region //描画

        myEngine->PreCommandSet(worldColor);



#ifdef _DEBUG
        grid.Draw(camera);
#endif // _DEBUG

        cube[0].PreDraw(kBlendModeNormal);
        cube[0].Draw(camera, MakeIdentity4x4(), lightType);
        cube[1].Draw(camera, cubeWorldTransform.matWorld_, lightType);

        MyEngine::SetBlendMode(blendMode);
        player->Draw(camera, lightType);
        MyEngine::SetBlendMode();

        sprites[0]->PreDraw(blendMode);

        for (Sprite* sprite : sprites) {
            sprite->Draw(cameraSprite, lightType);
        }

        particle.Draw(camera);

        myEngine->PostCommandSet();

#pragma endregion

    }

    for (Sprite* sprite : sprites) {
        delete sprite;
    }

    sprites.clear();

    myEngine->Finalize();

    return 0;
}
