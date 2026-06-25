#pragma once
#include<chrono>

class TimeManager {

public:
    static float DeltaTime();
    TimeManager();
    void Update();
private:
    std::chrono::steady_clock::time_point reference_;
    static float deltaTime_;

};