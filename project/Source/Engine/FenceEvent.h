#pragma once
#include <Windows.h>

class FenceEvent
{
public:
    void Create();
    HANDLE& GetEvent() {
        return fenceEvent_
            ;
    };
private:
    HANDLE fenceEvent_;
};

