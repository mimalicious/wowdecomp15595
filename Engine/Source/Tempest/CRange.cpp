#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CRange.h"
#include "Engine/Source/Tempest/CRandom.h"

// INLINED
float CRange::Random(CRndSeed& seed) {
    return CRandom::ufloat_(this->m_h - this->m_l, seed) + this->m_l;
}
