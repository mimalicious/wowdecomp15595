#include "Engine/Source/Gx/CGxDevice/EmergencyMem.h"

// INLINED
EmergencyMem::EmergencyMem() : m_lock(false) {
}

// OFFSET: %0x007A7830
void EmergencyMem::Unlock() {
    this->m_lock = false;
}

// OFFSET: %0x007AAC80
void* EmergencyMem::Lock(uint32_t size) {
    if (this->m_data.Count() < size) {
        this->m_data.SetCount(size);
    }

    void* data = this->m_data.Ptr();
    this->m_lock = true;

    return data;
}
