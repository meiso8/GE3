#include "DsvDescriptorHeap.h"

uint32_t DsvDescriptorHeap::useIndex_ = 0;
const uint32_t DsvDescriptorHeap::kMaxCount_ = 1;
uint32_t DsvDescriptorHeap::descriptorSize_ = 0;

DsvDescriptorHeap::DsvDescriptorHeap() {
    CreateDescriptorHeapAndSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxCount_,false);
}