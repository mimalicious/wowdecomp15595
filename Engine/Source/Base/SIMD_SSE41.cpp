#include <cstdint>
#include <smmintrin.h>
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"

// OFFSET: %0x00498EE0
static uint32_t PlaneMask_SSE41(int32_t count, const C4Plane* planes, const CAaBox* box) {
    __m128 one = _mm_set1_ps(1.0f);
    __m128 bottom = _mm_insert_ps(_mm_loadu_ps(&box->b.x), one, 0xF0);
    __m128 top = _mm_insert_ps(_mm_loadu_ps(&box->t.x), one, 0xF0);
    const __m128* plane = reinterpret_cast<const __m128*>(planes);
    uint32_t mask = 0;

    for (int32_t i = 0; i < count; i++, plane++) {
        mask = static_cast<uint32_t>(_mm_movemask_ps(_mm_dp_ps(*plane, _mm_blendv_ps(top, bottom, *plane), 0xF1))) | (mask << 1);
    }

    return mask ^ 0x3F;
}

// PROVEN: 46c85922 IDENTICAL/NONE
// OFFSET: 0x004910F0
void SIMD::Init_SSE41(SIMD::FunctionTable* table) {
    table->PlaneMask = PlaneMask_SSE41;
}
