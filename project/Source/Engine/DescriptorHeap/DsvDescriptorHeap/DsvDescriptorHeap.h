#pragma once
#include"../BaseDescriptor/BaseDescriptor.h"

class DsvDescriptorHeap:public BaseDescriptor
{
public:
    const  uint32_t kMaxDSVCount_ = 1;
public:
    DsvDescriptorHeap();
};

