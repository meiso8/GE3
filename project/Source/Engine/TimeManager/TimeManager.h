#pragma once
#include<chrono>

class TimeManager {

public:
    static float DeltaTime();
    static float GameTime();
    TimeManager();
    void Update();
private:
   std::chrono::steady_clock::time_point reference_;
    static float deltaTime_;
    static float gameTime_;

};