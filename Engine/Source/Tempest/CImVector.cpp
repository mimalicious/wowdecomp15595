#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CImVector.h"

// PROVEN: 6423c91f IDENTICAL/DIFFERENT
// OFFSET: 0x00464E50/0x140152A90
void CImVector::Set(float a, float r, float g, float b) {
    uint32_t value = static_cast<uint8_t>(CMath::ftol_round_0_255_(a));
    value = (value << 8) | static_cast<uint8_t>(CMath::ftol_round_0_255_(r));
    value = (value << 8) | static_cast<uint8_t>(CMath::ftol_round_0_255_(g));
    value = (value << 8) | static_cast<uint8_t>(CMath::ftol_round_0_255_(b));
    *reinterpret_cast<uint32_t*>(this) = value;
}

// PROVEN: 8f7b43c8 IDENTICAL/IDENTICAL
// OFFSET: 0x005AD230/0x14021F6C0
void CImVector::InterpColor(int32_t t, const CImVector* src) {
    uint32_t& value = *reinterpret_cast<uint32_t*>(this);

    if (t == 255) {
        value = (value & 0xFF000000) | (*reinterpret_cast<const uint32_t*>(src) & 0x00FFFFFF);
        return;
    }

    uint32_t r0 = this->r;
    uint32_t g0 = this->g;
    uint32_t b0 = this->b;

    uint8_t r = r0 + ((static_cast<uint32_t>(src->r - r0) * t) >> 8);
    uint8_t g = g0 + ((static_cast<uint32_t>(src->g - g0) * t) >> 8);
    uint8_t b = b0 + ((static_cast<uint32_t>(src->b - b0) * t) >> 8);

    value = ((((r << 8) | g) << 8) | b) | (value & 0xFF000000);
}

// OFFSET: #0x009851A0
CImVector& CImVector::operator=(const C3Vector& color) {
    this->a = 0xFF;
    this->r = static_cast<uint8_t>(CMath::fint_(color.x * 255.0f));
    this->g = static_cast<uint8_t>(CMath::fint_(color.y * 255.0f));
    this->b = static_cast<uint8_t>(CMath::fint_(color.z * 255.0f));
    return *this;
}

// OFFSET: #0x0057E480
void CImVector::InterpColorRGB(uint32_t t, const CImVector* src) {
    uint8_t r = this->r + ((t * (src->r - this->r)) >> 8);
    uint8_t g = this->g + ((t * (src->g - this->g)) >> 8);
    uint8_t b = this->b + ((t * (src->b - this->b)) >> 8);
    this->r = r;
    this->g = g;
    this->b = b;
}
