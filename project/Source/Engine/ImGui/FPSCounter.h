#pragma once
#include <chrono>
#include <iostream>

class FPSCounter {
public:
    void Update();
    int& GetFPS() { return fps_; }
private:
    std::chrono::high_resolution_clock::time_point lastTime_ = std::chrono::high_resolution_clock::now();
    double timeAccumulator_ = 0.0;
    int frameCount_ = 0;
    int fps_ = 0;
};
