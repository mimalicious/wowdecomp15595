#include "Engine/Source/Base/RefCount.h"

// OFFSET: 0x004093D0/0x14000BB00
void TRefCnt::DeleteSelf() {
    delete this;
}

// OFFSET: 0x004FF400/0x140159E70
TRefCnt::~TRefCnt() {
}
