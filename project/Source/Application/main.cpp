#include<numbers>
#include"MyEngine.h"
#include"Player.h"
#include"Particle/Particle.h"
#include"Lerp.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    MyEngine* myEngine = MyEngine::GetInstance();
    myEngine->Create(L"2102", WIN_WIDTH, WIN_HEIGHT);

    //音声の読み込み
    Sound::GetInstance()->Load();

    //デバック用
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

    Texture::GetInstance()->Load();
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
    cubeWorldTransform.scale_ = { 10.0f,10.0f,10.0f };
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

        if (Input::IsTriggerKey(DIK_SPACE) || Input::IsJoyStickTrigger(1)) {
            //デバッグの切り替え
            isDebug = (isDebug) ? false : true;
        }

        if (Input::IsTriggerKey(DIK_O)) {
            camera.SetOrthographic(camera.GetIsOrthographic() ? false : true);
        }

        if (Input::IsTriggerKey(DIK_A)) {
            SoundManager::Play(Sound::GetHandle(Sound::SE1), 1.0f, false);
        }

        if (Input::IsTriggerKey(DIK_S)) {
            SoundManager::Play(Sound::GetHandle(Sound::SE2), 1.0f, false);
        }

        if (!SoundManager::IsPlaying()) {
            SoundManager::Play(Sound::GetHandle(Sound::BGM1), 0.0625f, true);
        }

        if (isDebug) {
            debugUI.CheckDirectionalLight(lightType);
            debugUI.CheckFPS();

            debugUI.CheckInput(*Input::GetInstance());
            debugUI.CheckColor(worldColor);
            debugUI.CheckCamera(camera);

            debugUI.CheckBlendMode(blendMode);
            debugUI.CheckSprite(*sprites[0]);

            //視点操作
            Input::EyeOperation(camera);

        } else {

            Vector3 cameraPos = player->GetTranslate();

            Vector2 delta = { 0.0f,0.0f };
            if (Input::GetJoyStickPos(&delta.x, &delta.y, Input::BUTTON_RIGHT)) {
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
