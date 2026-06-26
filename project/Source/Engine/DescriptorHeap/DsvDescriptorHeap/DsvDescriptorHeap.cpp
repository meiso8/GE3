#include "DsvDescriptorHeap.h"

DsvDescriptorHeap::DsvDescriptorHeap() {
    CreateDescriptorHeapAndSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV,false, kMaxDSVCount_);
}