#include "FenceEvent.h"
#include<cassert>

void FenceEvent::Create() {

    fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent_ != nullptr);

}