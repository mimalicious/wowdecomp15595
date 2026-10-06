#include "Engine/Source/Base/RefCount.h"

// INLINED
TRefCnt::TRefCnt() {
    this->m_refcnt = 0;
}

// OFFSET: %0x00408B90
void TRefCnt::DeleteSelf() {
    delete this;
}

// OFFSET: %0x0050F5B0
TRefCnt::~TRefCnt() {
}
