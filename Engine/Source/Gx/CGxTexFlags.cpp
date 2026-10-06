#include <cstdint>
#include <cstring>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"

// OFFSET: %0x007A62E0
CGxTexFlags::CGxTexFlags() {
    *reinterpret_cast<uint32_t*>(this) = (*reinterpret_cast<uint32_t*>(this) & 0x100000) | 0x1001;
}

// INLINED
CGxTexFlags::CGxTexFlags(EGxTexFilter filter, uint32_t wrapU, uint32_t wrapV, uint32_t wrapW, uint32_t renderTarget, uint32_t maxAnisotropy, uint32_t bit8, uint32_t bit14, uint32_t bit15) {
    *reinterpret_cast<uint32_t*>(this) = (*reinterpret_cast<uint32_t*>(this) & 0x100000) | 0x1001;
    this->m_filter = filter;
    this->m_wrapU = wrapU;
    this->m_wrapV = wrapV;
    this->m_wrapW = wrapW;
    this->m_renderTarget = renderTarget;
    this->m_bit8 = bit8;
    this->m_maxAnisotropy = maxAnisotropy < g_theGxDevicePtr->Caps().m_maxTexAnisotropy ? maxAnisotropy : g_theGxDevicePtr->Caps().m_maxTexAnisotropy;
    this->m_bit14 = bit14;
    this->m_bit15 = bit15;
}

// PROVEN: d5f3f0a2 IDENTICAL/NONE
// OFFSET: 0x00842B30
bool CGxTexFlags::operator!=(const CGxTexFlags& flags) const {
    return memcmp(this, &flags, sizeof(CGxTexFlags)) != 0;
}

// PROVEN: f58a0011 IDENTICAL/NONE
// OFFSET: 0x00541160
bool CGxTexFlags::operator==(const CGxTexFlags& flags) const {
    return memcmp(this, &flags, sizeof(CGxTexFlags)) == 0;
}
