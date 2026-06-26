#pragma once

#include"../BaseDescriptor/BaseDescriptor.h"

class RtvDescriptorHeap :public BaseDescriptor
{
public:
    const uint32_t kMaxRTVCount_ = 64;
public:
    RtvDescriptorHeap();

};

