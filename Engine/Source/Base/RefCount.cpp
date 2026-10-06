#include "Engine/Source/Base/RefCount.h"

// INLINED
TRefCnt::TRefCnt() {
    this->m_refcnt = 0;
}

// PROVEN: 75e498ac IDENTICAL/IDENTICAL
// OFFSET: 0x004093D0/0x14000BB00
void TRefCnt::DeleteSelf() {
    delete this;
}

// OFFSET: %0x0050F5B0
TRefCnt::~TRefCnt() {
}
