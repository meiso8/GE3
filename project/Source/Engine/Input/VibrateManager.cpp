#include "VibrateManager.h"
#include"TimeManager.h"
#include"Log.h"

float VibrateManager:: timer_ = 0.0f;
WORD  VibrateManager::leftMotor_ = 0;
WORD  VibrateManager::rightMotor_ = 0;

VibrateManager::VibrateManager()
{
    Initialize();
    LogFile::Log("Create VibrateManager\n");
}

void VibrateManager::Initialize()
{
   timer_ = 0.0f;
   leftMotor_ = 0;
   rightMotor_ = 0;

   LogFile::Log("VibrateManager Initialize\n");
}

void VibrateManager::Finalize()
{
    Input::VibrateController(0, 0, 0);
    LogFile::Log("VibrateManager Finalize\n");
}

void VibrateManager::Update()
{

    if (timer_ == 0.0f) {
        return;
    }

    if (timer_ <= 0.0f) {
        timer_ = 0.0f;
        leftMotor_ = 0;
        rightMotor_ = 0;
    } else {
        timer_ -= TimeManager::DeltaTime();
    }

    Input::VibrateController(0, leftMotor_, rightMotor_);

}

void VibrateManager::SetTime(const float& timer, WORD leftMotor, WORD rightMotor)
{
    timer_ = timer;
    leftMotor_ = leftMotor;
    rightMotor_ = rightMotor;

}
