#include <cstdint>
#include <emmintrin.h>
#include "Engine/Source/Base/SIMD.h"

// INLINED
static __m128 Channel_SSE2(__m128i words, __m128 scale, __m128 mean) {
    return _mm_sub_ps(_mm_mul_ps(_mm_cvtepi32_ps(words), scale), mean);
}

// INLINED
static uint32_t Pack565_SSE2(uint32_t color) {
    return ((color & 0xF8) | (((color & 0xFC00) | ((color >> 3) & 0x1F0000)) >> 2)) >> 3;
}

// OFFSET: 0x00491100/0x1400B6060
static void CompressDXT1_SSE2(const uint8_t* src, int32_t pitch, uint8_t* dst) {
    __m128i zero = _mm_setzero_si128();
    __m128i row0 = _mm_load_si128(reinterpret_cast<const __m128i*>(src));
    __m128i row1 = _mm_load_si128(reinterpret_cast<const __m128i*>(src + pitch));
    __m128i row2 = _mm_load_si128(reinterpret_cast<const __m128i*>(src + 2 * pitch));
    __m128i row3 = _mm_load_si128(reinterpret_cast<const __m128i*>(src + 3 * pitch));

    __m128i sum = _mm_add_epi16(_mm_unpacklo_epi8(row0, zero), _mm_unpackhi_epi8(row0, zero));
    sum = _mm_add_epi16(sum, _mm_unpacklo_epi8(row1, zero));
    sum = _mm_add_epi16(sum, _mm_unpackhi_epi8(row1, zero));
    sum = _mm_add_epi16(sum, _mm_unpacklo_epi8(row2, zero));
    sum = _mm_add_epi16(sum, _mm_unpackhi_epi8(row2, zero));
    sum = _mm_add_epi16(sum, _mm_unpacklo_epi8(row3, zero));
    sum = _mm_add_epi16(sum, _mm_unpackhi_epi8(row3, zero));

    __m128 scale = _mm_set1_ps(0.0039215689f);
    __m128 mean = _mm_mul_ps(_mm_cvtepi32_ps(_mm_unpacklo_epi16(_mm_srli_epi16(_mm_add_epi16(sum, _mm_shuffle_epi32(sum, 0x4E)), 4), zero)), scale);

    __m128 covX = _mm_setzero_ps();
    __m128 covY = _mm_setzero_ps();
    __m128 covZ = _mm_setzero_ps();
    const uint8_t* line = src;

    for (int32_t y = 0; y < 4; y++, line += pitch) {
        __m128i row = _mm_load_si128(reinterpret_cast<const __m128i*>(line));
        __m128i high = _mm_unpackhi_epi8(row, zero);
        __m128i low = _mm_unpacklo_epi8(row, zero);
        __m128 d3 = Channel_SSE2(_mm_unpackhi_epi16(high, zero), scale, mean);
        __m128 d2 = Channel_SSE2(_mm_unpacklo_epi16(high, zero), scale, mean);
        __m128 d1 = Channel_SSE2(_mm_unpackhi_epi16(low, zero), scale, mean);
        __m128 d0 = Channel_SSE2(_mm_unpacklo_epi16(low, zero), scale, mean);

        covX = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(d3, d3, 0x00), d3), covX), _mm_mul_ps(_mm_shuffle_ps(d2, d2, 0x00), d2)), _mm_mul_ps(_mm_shuffle_ps(d1, d1, 0x00), d1)), _mm_mul_ps(_mm_shuffle_ps(d0, d0, 0x00), d0));
        covY = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(d3, d3, 0x55), d3), covY), _mm_mul_ps(_mm_shuffle_ps(d2, d2, 0x55), d2)), _mm_mul_ps(_mm_shuffle_ps(d1, d1, 0x55), d1)), _mm_mul_ps(_mm_shuffle_ps(d0, d0, 0x55), d0));
        covZ = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(d3, d3, 0xAA), d3), covZ), _mm_mul_ps(_mm_shuffle_ps(d2, d2, 0xAA), d2)), _mm_mul_ps(_mm_shuffle_ps(d1, d1, 0xAA), d1)), _mm_mul_ps(_mm_shuffle_ps(d0, d0, 0xAA), d0));
    }

    __m128 axis = _mm_set1_ps(1.0f);

    for (int32_t i = 0; i < 8; i++) {
        __m128 v = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(axis, axis, 0xAA), covZ), _mm_mul_ps(_mm_shuffle_ps(axis, axis, 0x55), covY)), _mm_mul_ps(_mm_shuffle_ps(axis, axis, 0x00), covX));
        __m128 largest = _mm_max_ps(v, _mm_shuffle_ps(v, v, 0xB1));
        largest = _mm_max_ps(largest, _mm_shuffle_ps(largest, largest, 0x4E));
        axis = _mm_mul_ps(_mm_and_ps(_mm_cmpneq_ps(_mm_setzero_ps(), largest), _mm_rcp_ps(largest)), v);
    }

    __m128 lengthSq = _mm_mul_ps(axis, axis);
    lengthSq = _mm_add_ps(_mm_shuffle_ps(lengthSq, lengthSq, 0xB1), lengthSq);
    lengthSq = _mm_add_ps(_mm_shuffle_ps(lengthSq, lengthSq, 0x4E), lengthSq);
    axis = _mm_mul_ps(_mm_and_ps(_mm_cmpneq_ps(_mm_setzero_ps(), lengthSq), _mm_rsqrt_ps(lengthSq)), axis);

    __m128 low = _mm_set_ss(1.0f);
    __m128 high = _mm_set_ss(-1.0f);
    __m128 meanX = _mm_shuffle_ps(mean, mean, 0x00);
    __m128 meanY = _mm_shuffle_ps(mean, mean, 0x55);
    __m128 meanZ = _mm_shuffle_ps(mean, mean, 0xAA);
    __m128 axisX = _mm_shuffle_ps(axis, axis, 0x00);
    __m128 axisY = _mm_shuffle_ps(axis, axis, 0x55);
    __m128 axisZ = _mm_shuffle_ps(axis, axis, 0xAA);
    __m128 projections[4];
    line = src;

    for (int32_t y = 0; y < 4; y++, line += pitch) {
        __m128i row = _mm_load_si128(reinterpret_cast<const __m128i*>(line));
        __m128i interleaved = _mm_unpackhi_epi8(_mm_shuffle_epi32(row, 0x8D), _mm_shuffle_epi32(row, 0xD8));
        __m128i planar = _mm_unpackhi_epi16(_mm_slli_si128(interleaved, 8), interleaved);
        __m128i blueGreen = _mm_unpacklo_epi8(planar, zero);
        __m128 t = _mm_add_ps(_mm_mul_ps(Channel_SSE2(_mm_unpacklo_epi16(blueGreen, zero), scale, meanX), axisX), _mm_mul_ps(Channel_SSE2(_mm_unpackhi_epi16(blueGreen, zero), scale, meanY), axisY));
        t = _mm_add_ps(t, _mm_mul_ps(Channel_SSE2(_mm_unpacklo_epi16(_mm_unpackhi_epi8(planar, zero), zero), scale, meanZ), axisZ));
        projections[y] = t;
        low = _mm_min_ps(low, t);
        high = _mm_max_ps(high, t);
    }

    low = _mm_min_ps(low, _mm_shuffle_ps(low, low, 0xB1));
    high = _mm_max_ps(high, _mm_shuffle_ps(high, high, 0xB1));
    low = _mm_min_ps(low, _mm_shuffle_ps(low, low, 0x4E));
    high = _mm_max_ps(high, _mm_shuffle_ps(high, high, 0x4E));

    __m128 range = _mm_set1_ps(255.0f);
    __m128i endpoints = _mm_packs_epi32(_mm_cvtps_epi32(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(low, axis), mean), range)), _mm_cvtps_epi32(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(high, axis), mean), range)));
    uint32_t color0 = Pack565_SSE2(static_cast<uint32_t>(_mm_cvtsi128_si32(_mm_packus_epi16(_mm_srli_si128(endpoints, 8), zero))));
    uint32_t color1 = Pack565_SSE2(static_cast<uint32_t>(_mm_cvtsi128_si32(_mm_packus_epi16(endpoints, zero))));
    uint32_t bits = 0;

    if (color0 != color1) {
        __m128 step = _mm_div_ps(_mm_set1_ps(3.0f), _mm_sub_ps(high, low));
        __m128i q0 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_mul_ps(_mm_sub_ps(projections[0], low), step)), _mm_cvtps_epi32(_mm_mul_ps(_mm_sub_ps(projections[1], low), step)));
        __m128i q1 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_mul_ps(_mm_sub_ps(projections[2], low), step)), _mm_cvtps_epi32(_mm_mul_ps(_mm_sub_ps(projections[3], low), step)));
        __m128i one = _mm_set1_epi16(1);
        __m128i indices = _mm_packs_epi16(_mm_add_epi16(q0, _mm_srli_epi16(_mm_add_epi16(q0, one), 1)), _mm_add_epi16(q1, _mm_srli_epi16(_mm_add_epi16(q1, one), 1)));
        uint8_t values[16];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(values), indices);

        for (int32_t i = 0; i < 16; i++) {
            bits |= static_cast<uint32_t>(values[i] & 3) << (2 * i);
        }

        bits ^= 0x55555555;

        if (color0 < color1) {
            uint32_t swap = color0;
            color0 = color1;
            color1 = swap;
            bits ^= 0x55555555;
        }
    }

    reinterpret_cast<uint32_t*>(dst)[0] = color0 | (color1 << 16);
    reinterpret_cast<uint32_t*>(dst)[1] = bits;
}

// OFFSET: 0x00491740/0x1400B66D0
void SIMD::Init_SSE2(SIMD::FunctionTable* table) {
    table->CompressDXT1 = CompressDXT1_SSE2;
}
