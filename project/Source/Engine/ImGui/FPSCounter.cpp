#include "FPSCounter.h"

void FPSCounter::Update() {
    auto now = std::chrono::high_resolution_clock::now();
    double frameTime = std::chrono::duration<double>(now - lastTime_).count();
    lastTime_ = now;

    frameCount_++;
    timeAccumulator_ += frameTime;

    if (timeAccumulator_ >= 1.0) {
        fps_ = frameCount_;
        frameCount_ = 0;
        timeAccumulator_ = 0.0;
        std::cout << "FPS: " << fps_ << std::endl;
    }
}