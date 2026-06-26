#include "RtvDescriptorHeap.h"
#include"Log.h"

RtvDescriptorHeap::RtvDescriptorHeap()
{
    CreateDescriptorHeapAndSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, kMaxRTVCount_);
    LogFile::Log("Create　Rtv Descriptor Heap\n");
}
