#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "Engine/Source/Base/Status.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"

// STATIC: 0x00BC7EB0
static uint32_t s_nibbleMasks[2] = { 0x0F, 0xF0 };

// STATIC: 0x00BC7EB8
static uint32_t s_nibblePackShifts[2] = { 4, 0 };

// STATIC: 0x00BC7EC0
static uint32_t s_nibbleShifts[2] = { 0, 4 };

// STATIC: 0x00BC7EC8
static uint32_t s_bitMasks[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

// STATIC: 0x00BC7EE8
static uint32_t s_bitShifts[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

// STATIC: 0x00BC7F08
static uint8_t s_shadowAlpha[2] = { 0xFF, 0x00 };

// STATIC: 0x00D01FB4
static uint16_t s_shadowTexels[2] = { 0x0000, 0xFFFF };

static inline uint32_t AlphaNibble(const uint8_t* alpha, uint32_t index) {
    return (s_nibbleMasks[index & 1] & alpha[index >> 1]) >> s_nibbleShifts[index & 1];
}

static inline uint32_t ShadowBit(const uint8_t* shadow, uint32_t index) {
    return (s_bitMasks[index & 7] & shadow[index >> 3]) >> s_bitShifts[index & 7];
}

static inline uint32_t PackNibble(const uint8_t* alpha, uint32_t index) {
    return (s_nibbleMasks[index & 1] & alpha[index >> 1]) << s_nibblePackShifts[index & 1];
}

// OFFSET: 0x007660E0
static const uint8_t* DecompressAlphaRLE(const uint8_t* src, uint8_t* dest, int32_t count) {
    int32_t written = 0;
    int32_t read = 0;

    if (count <= 0) {
        return src;
    }

    while (true) {
        int8_t header = static_cast<int8_t>(src[read]);
        read++;
        uint32_t run = static_cast<uint8_t>(header);

        if (header < 0) {
            uint8_t value = src[read];
            run = run & 0x7F;
            read++;

            while (run) {
                if (written >= count) {
                    return src + read;
                }

                dest[written++] = value;
                run--;
            }
        } else {
            while (run) {
                if (written >= count) {
                    return src + read;
                }

                dest[written++] = src[read++];
                run--;
            }
        }

        if (written >= count) {
            return src + read;
        }
    }
}

// OFFSET: 0x00766150
static void NextAlphaRows8888(const uint8_t*& shadowRow, const uint8_t*& shadow, CMapRenderChunk::SLayerAlpha* alpha, uint32_t size, const uint8_t** rows) {
    if (shadow) {
        shadowRow = shadow;
        shadow += size >> 3;
    } else {
        shadowRow = CMapRenderChunk::s_emptyShadow;
    }

    uint8_t* buffer = CMapRenderChunk::s_alphaRowBuffer;

    for (uint32_t i = 0; i < 4; i++, buffer += 64) {
        if (!alpha[i].m_alpha) {
            rows[i] = CMapRenderChunk::s_emptyAlpha;
        } else if (alpha[i].m_flags & 0x200) {
            rows[i] = buffer;
            alpha[i].m_alpha = DecompressAlphaRLE(alpha[i].m_alpha, buffer, size);
        } else {
            rows[i] = alpha[i].m_alpha;
            alpha[i].m_alpha += size;
        }
    }
}

// OFFSET: 0x007661D0
static void NextAlphaRows4444(CMapRenderChunk::SLayerAlpha* alpha, uint32_t size, const uint8_t*& shadowRow, const uint8_t** rows, const uint8_t*& shadow) {
    if (shadow) {
        shadowRow = shadow;
        shadow += size >> 3;
    } else {
        shadowRow = CMapRenderChunk::s_emptyShadow;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (alpha[i].m_alpha) {
            rows[i] = alpha[i].m_alpha;
            alpha[i].m_alpha += size >> 1;
        } else {
            rows[i] = CMapRenderChunk::s_emptyAlpha;
        }
    }
}

// OFFSET: 0x00766250
static void UnpackAlphaBitsFixed4444Mip0(uint16_t* dest, uint32_t size, const CMapRenderChunk::SLayerAlpha* alpha) {
    const uint8_t* bits = alpha->m_alpha;
    uint32_t index = 0;

    for (uint32_t y = 0; y < size; y++) {
        for (uint32_t x = 0; x < size; x++, index++) {
            *dest++ = static_cast<uint16_t>((AlphaNibble(bits, index) << 12) | 0xFFF);
        }
    }
}

// OFFSET: 0x007662C0
static void UnpackAlphaBitsFixed4444Mip1(uint16_t* dest, uint32_t size, const CMapRenderChunk::SLayerAlpha* alpha) {
    const uint8_t* bits = alpha->m_alpha;
    uint32_t index = 0;

    for (uint32_t y = 0; y < size; y += 2) {
        if (size != 2) {
            for (uint32_t x = 0; x < ((size - 3) >> 1) + 1; x++, index += 2) {
                *dest++ = static_cast<uint16_t>((AlphaNibble(bits, index) << 12) | 0xFFF);
            }
        }

        uint32_t last = index + 1;
        *dest++ = static_cast<uint16_t>((AlphaNibble(bits, last) << 12) | 0xFFF);
        uint32_t next = last + 1;

        if (y == size - 4) {
            next += 64;
        }

        index = size + next;
    }
}

// OFFSET: 0x00766390
static void UnpackAlphaBitsUnfixed4444Mip0(uint16_t* dest, uint32_t size, const CMapRenderChunk::SLayerAlpha* alpha) {
    const uint8_t* bits = alpha->m_alpha;
    uint32_t index = 0;
    uint32_t count = size - 1;
    uint16_t texel = 0xFFF;

    if (size != 1) {
        for (uint32_t y = 0; y < size - 1; y++) {
            for (uint32_t x = 0; x < count; x++, index++) {
                texel = static_cast<uint16_t>((AlphaNibble(bits, index) << 12) | 0xFFF);
                *dest++ = texel;
            }

            *dest++ = texel;
            index++;
        }
    }

    for (uint32_t i = index - size, x = 0; x < count; x++, i++) {
        texel = static_cast<uint16_t>((AlphaNibble(bits, i) << 12) | 0xFFF);
        *dest++ = texel;
    }

    *dest = texel;
}

// OFFSET: 0x00766460
static void UnpackAlphaBitsUnfixed4444Mip1(uint16_t* dest, uint32_t size, const CMapRenderChunk::SLayerAlpha* alpha) {
    const uint8_t* bits = alpha->m_alpha;
    uint32_t index = 0;

    if (size == 1) {
        return;
    }

    for (uint32_t y = ((size - 2) >> 1) + 1; y; y--) {
        if (size != 1) {
            for (uint32_t x = ((size - 2) >> 1) + 1; x; x--, index += 2) {
                *dest++ = static_cast<uint16_t>((AlphaNibble(bits, index) << 12) | 0xFFF);
            }
        }

        index += size;
    }
}

// OFFSET: 0x007664F0
static void GatherLayerAlphas(const CMapChunk* chunk, CMapRenderChunk::SLayerAlpha* alpha) {
    uint32_t count = chunk->m_header.m_nLayers;

    for (uint32_t i = 0; i < 4; i++) {
        alpha[i].m_alpha = NULL;

        if (count > i) {
            const SMLayer& layer = chunk->m_layers[i];

            if (layer.m_flags & 0x100) {
                alpha[i].m_alpha = chunk->m_alphaMaps + layer.m_offsetInMCAL;
            }

            alpha[i].m_flags = layer.m_flags;
        }
    }
}

// OFFSET: 0x007665C0
static void UnpackShadowBitsMip0(uint16_t* dest, uint32_t size, const uint8_t* shadow) {
    uint32_t index = 0;

    for (uint32_t y = 0; y < size; y++) {
        for (uint32_t x = 0; x < size; x++, index++) {
            *dest++ = s_shadowTexels[ShadowBit(shadow, index)];
        }
    }
}

// OFFSET: 0x00766630
static void UnpackShadowBitsMip1(uint16_t* dest, uint32_t size, const uint8_t* shadow) {
    uint32_t index = 0;

    for (uint32_t y = 0; y < size; y += 2) {
        if (size != 2) {
            for (uint32_t x = 0; x < ((size - 3) >> 1) + 1; x++, index += 2) {
                *dest++ = s_shadowTexels[ShadowBit(shadow, index)];
            }
        }

        uint32_t last = index + 1;
        *dest++ = s_shadowTexels[ShadowBit(shadow, last)];
        uint32_t next = last + 1;

        if (y == size - 4) {
            next += size;
        }

        index = size + next;
    }
}

static inline uint32_t PackAlphaShadow8888(const uint8_t* const* rows, const uint8_t* shadowRow, uint32_t index, int32_t layer) {
    uint32_t values[4];
    values[1] = rows[1][index];
    values[2] = rows[2][index];
    values[3] = rows[3][index];

    if (layer) {
        values[layer] = 255 - values[3] - values[2] - values[1] - rows[0][index];
    }

    return (values[3] & 0xFF) | ((values[2] & 0xFF) << 8) | ((values[1] & 0xFF) << 16) | (static_cast<uint32_t>(s_shadowAlpha[ShadowBit(shadowRow, index)]) << 24);
}

// OFFSET: 0x007669D0
static void UnpackAlphaShadowBitsFixed8888Mip0(uint32_t* dest, uint32_t x, uint32_t pitch, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, int32_t layer, const uint8_t* shadow) {
    uint32_t* row = dest + x;

    for (uint32_t y = 0; y < size; y++) {
        const uint8_t* shadowRow;
        const uint8_t* rows[4];
        NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);

        for (uint32_t i = 0; i < size; i++) {
            row[i] = PackAlphaShadow8888(rows, shadowRow, i, layer);
        }

        row += pitch;
    }
}

// OFFSET: 0x00766B30
static void UnpackAlphaShadowBitsFixed8888Mip1(uint32_t* dest, uint32_t x, uint32_t pitch, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, int32_t layer, const uint8_t* shadow) {
    uint32_t* row = dest + x;

    for (uint32_t y = 0; y < size; y += 2) {
        const uint8_t* shadowRow;
        const uint8_t* rows[4];
        NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);

        uint32_t i = 0;
        uint32_t out = 0;

        for (; i < size - 2; i += 2, out++) {
            row[out] = PackAlphaShadow8888(rows, shadowRow, i, layer);
        }

        row[out] = PackAlphaShadow8888(rows, shadowRow, i + 1, layer);

        if (y < size - 2) {
            NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);
        }

        if (y == size - 4) {
            NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);
        }

        row += pitch;
    }
}

static inline uint16_t PackAlphaShadow4444(const uint8_t* const* rows, const uint8_t* shadowRow, uint32_t index) {
    uint32_t shift = s_nibblePackShifts[index & 1];
    uint32_t mask = s_nibbleMasks[index & 1];
    uint32_t a0 = (mask & rows[1][index >> 1]) << shift;
    uint32_t a1 = ((mask & rows[2][index >> 1]) << shift) & 0xF0;
    uint32_t a2 = static_cast<uint8_t>((mask & rows[3][index >> 1]) << shift) >> 4;
    uint32_t s = (s_shadowAlpha[ShadowBit(shadowRow, index)] & 0xF0) << 8;
    return static_cast<uint16_t>(a2 | a1 | (16 * (a0 & 0xF0)) | s);
}

// OFFSET: 0x00766DE0
static void UnpackAlphaShadowBitsFixed4444Mip0(uint16_t* dest, uint32_t x, uint32_t pitch, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, const uint8_t* shadow) {
    uint16_t* row = dest + x;

    for (uint32_t y = 0; y < size; y++) {
        const uint8_t* shadowRow;
        const uint8_t* rows[4];
        NextAlphaRows4444(alpha, size, shadowRow, rows, shadow);

        for (uint32_t i = 0; i < size; i++) {
            row[i] = PackAlphaShadow4444(rows, shadowRow, i);
        }

        row += pitch;
    }
}

// OFFSET: 0x00766F10
static void UnpackAlphaShadowBitsFixed4444Mip1(uint16_t* dest, uint32_t x, uint32_t pitch, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, const uint8_t* shadow) {
    uint16_t* row = dest + x;

    for (uint32_t y = 0; y < size; y += 2) {
        const uint8_t* shadowRow;
        const uint8_t* rows[4];
        NextAlphaRows4444(alpha, size, shadowRow, rows, shadow);

        uint32_t i = 0;
        uint32_t out = 0;

        if (size != 2) {
            for (; i < size - 2; i += 2, out++) {
                row[out] = PackAlphaShadow4444(rows, shadowRow, i);
            }
        }

        row[out] = PackAlphaShadow4444(rows, shadowRow, i + 1);

        NextAlphaRows4444(alpha, size, shadowRow, rows, shadow);

        if (y == size - 4) {
            NextAlphaRows4444(alpha, size, shadowRow, rows, shadow);
        }

        row += pitch;
    }
}

// OFFSET: 0x00767240
static void UnpackAlphaShadowRowUnfixed4444(uint16_t* dest, const uint8_t* const* rows, const uint8_t* shadowRow, uint32_t size) {
    uint32_t i = 0;
    uint16_t texel = 0;

    if (size == 1) {
        *dest = static_cast<uint16_t>(reinterpret_cast<uintptr_t>(dest));
        return;
    }

    do {
        texel = PackAlphaShadow4444(rows, shadowRow, i);
        dest[i] = texel;
        i++;
    } while (i < size - 1);

    dest[i] = texel;
}

// OFFSET: 0x00767340
static void UnpackAlphaShadowBitsUnfixed4444Mip0(uint16_t* dest, uint32_t x, uint32_t pitch, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, const uint8_t* shadow) {
    uint16_t* row = dest + x;
    const uint8_t* shadowRow = NULL;
    const uint8_t* rows[4];

    if (size != 1) {
        for (uint32_t y = size - 1; y; y--) {
            NextAlphaRows4444(alpha, size, shadowRow, rows, shadow);
            UnpackAlphaShadowRowUnfixed4444(row, rows, shadowRow, size);
            row += pitch;
        }
    }

    UnpackAlphaShadowRowUnfixed4444(row, rows, shadowRow, size);
}

// OFFSET: 0x007673C0
static void UnpackAlphaShadowBitsUnfixed4444Mip1(uint16_t* dest, uint32_t x, uint32_t pitch, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, const uint8_t* shadow) {
    uint16_t* row = dest + x;

    if (size == 1) {
        return;
    }

    for (uint32_t y = ((size - 2) >> 1) + 1; y; y--) {
        const uint8_t* shadowRow;
        const uint8_t* rows[4];
        NextAlphaRows4444(alpha, size, shadowRow, rows, shadow);

        if (size != 1) {
            uint16_t* out = row;

            for (uint32_t i = 0; i < size - 1; i += 2) {
                *out++ = PackAlphaShadow4444(rows, shadowRow, i);
            }
        }

        NextAlphaRows4444(alpha, size, shadowRow, rows, shadow);
        row += pitch;
    }
}

static inline uint32_t AlphaToTexel(uint32_t value) {
    return value | (value << 8) | (value << 16) | (value << 24);
}

// OFFSET: 0x00767670
static void RecreateAlphaBitsFixed8888Mip0(uint32_t* dest, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, const uint8_t* shadow) {
    for (uint32_t y = 0; y < size; y++) {
        const uint8_t* shadowRow;

        if (shadow) {
            shadowRow = shadow;
            shadow += size >> 3;
        } else {
            shadowRow = CMapRenderChunk::s_emptyShadow;
        }

        const uint8_t* row;

        if (alpha->m_flags & 0x200) {
            row = CMapRenderChunk::s_alphaRowBuffer;
            alpha->m_alpha = DecompressAlphaRLE(alpha->m_alpha, CMapRenderChunk::s_alphaRowBuffer, size);
        } else {
            row = alpha->m_alpha;
            alpha->m_alpha = row + size;
        }

        for (uint32_t x = 0; x < size; x++) {
            uint32_t value = row[x];

            if (ShadowBit(shadowRow, x)) {
                value = (178 * value) >> 8;
            }

            *dest++ = AlphaToTexel(value & 0xFF);
        }
    }
}

// OFFSET: 0x00767750
static void RecreateAlphaBitsFixed8888Mip1(uint32_t* dest, uint32_t size, CMapRenderChunk::SLayerAlpha* alpha, const uint8_t* shadow) {
    for (uint32_t y = 0; y < size; y += 2) {
        const uint8_t* shadowRow;

        if (shadow) {
            shadowRow = shadow;
            shadow += size >> 3;
        } else {
            shadowRow = CMapRenderChunk::s_emptyShadow;
        }

        const uint8_t* row;

        if (alpha->m_flags & 0x200) {
            row = CMapRenderChunk::s_alphaRowBuffer;
            alpha->m_alpha = DecompressAlphaRLE(alpha->m_alpha, CMapRenderChunk::s_alphaRowBuffer, size);
        } else {
            row = alpha->m_alpha;
            alpha->m_alpha = row + size;
        }

        uint32_t x = 0;

        if (size != 2) {
            for (; x < size - 2; x += 2) {
                uint32_t value = row[x];

                if (ShadowBit(shadowRow, x)) {
                    value = (178 * value) >> 8;
                }

                *dest++ = AlphaToTexel(value & 0xFF);
            }
        }

        uint32_t value = row[x + 1];

        if (ShadowBit(shadowRow, x + 1)) {
            value = (178 * value) >> 8;
        }

        *dest++ = AlphaToTexel(value & 0xFF);

        if (y < size - 2) {
            if (shadow) {
                shadow += size >> 3;
            }

            if (alpha->m_flags & 0x200) {
                alpha->m_alpha = DecompressAlphaRLE(alpha->m_alpha, CMapRenderChunk::s_alphaRowBuffer, size);
            } else {
                alpha->m_alpha += size;
            }
        }

        if (y == size - 4) {
            if (shadow) {
                shadow += size >> 3;
            }

            if (alpha->m_flags & 0x200) {
                alpha->m_alpha = DecompressAlphaRLE(alpha->m_alpha, CMapRenderChunk::s_alphaRowBuffer, size);
            } else {
                alpha->m_alpha += size;
            }
        }
    }
}

// OFFSET: 0x00767900
static void UnpackAlphaBitsFixed8888Mip0(CMapRenderChunk::SLayerAlpha* alpha, uint32_t* dest, uint32_t size, const uint8_t* shadow) {
    for (uint32_t y = 0; y < size; y++) {
        const uint8_t* shadowRow;
        const uint8_t* rows[4];
        NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);

        for (uint32_t x = 0; x < size; x++) {
            uint32_t value = static_cast<uint8_t>(255 - rows[1][x] - rows[2][x] - rows[3][x]);

            if (ShadowBit(shadowRow, x)) {
                value = (178 * value) >> 8;
            }

            *dest++ = AlphaToTexel(value & 0xFF);
        }
    }
}

// OFFSET: 0x007679E0
static void UnpackAlphaBitsFixed8888Mip1(CMapRenderChunk::SLayerAlpha* alpha, uint32_t* dest, uint32_t size, const uint8_t* shadow) {
    for (uint32_t y = 0; y < size; y += 2) {
        const uint8_t* shadowRow;
        const uint8_t* rows[4];
        NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);

        uint32_t x = 0;

        if (size != 2) {
            for (; x < size - 2; x += 2) {
                uint32_t value = static_cast<uint8_t>(255 - rows[1][x] - rows[2][x] - rows[3][x]);

                if (ShadowBit(shadowRow, x)) {
                    value = (178 * value) >> 8;
                }

                *dest++ = AlphaToTexel(value & 0xFF);
            }
        }

        uint32_t last = x + 1;
        uint32_t value = static_cast<uint8_t>(255 - rows[3][last] - rows[1][last] - rows[2][last]);

        if (ShadowBit(shadowRow, last)) {
            value = (178 * value) >> 8;
        }

        *dest++ = AlphaToTexel(value & 0xFF);

        if (y < size - 2) {
            NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);
        }

        if (y == size - 4) {
            NextAlphaRows8888(shadowRow, shadow, alpha, size, rows);
        }
    }
}

// OFFSET: 0x00766960
void CMapRenderChunk::UnpackShadowBits(CMapRenderChunkBatch* batch) {
    CMapChunk* chunk = this->GetBatchChunk(batch->m_chunkMask);
    const uint8_t* shadow = chunk->m_shadowMap;
    const SMAreaHeader* header = chunk->m_area->m_header;
    uint32_t size = 64;

    if (header) {
        size = 64 >> header->m_mamp;
    }

    if (this->m_flags & 0x8) {
        UnpackShadowBitsMip1(reinterpret_cast<uint16_t*>(CMapRenderChunk::s_texelBuffer), size, shadow);
    } else {
        UnpackShadowBitsMip0(reinterpret_cast<uint16_t*>(CMapRenderChunk::s_texelBuffer), size, shadow);
    }
}

// OFFSET: 0x00767590
void CMapRenderChunk::UnpackAlphaShadowBits(void* dest, uint32_t x, uint32_t pitch, uint32_t size, SLayerAlpha* alpha, int32_t layer, const uint8_t* shadow, EGxTexFormat format, int32_t fixedAlpha) {
    if (fixedAlpha) {
        if (format == 3) {
            if (this->m_flags & 0x8) {
                UnpackAlphaShadowBitsFixed4444Mip1(static_cast<uint16_t*>(dest), x, pitch, size, alpha, shadow);
            } else {
                UnpackAlphaShadowBitsFixed4444Mip0(static_cast<uint16_t*>(dest), x, pitch, size, alpha, shadow);
            }
        } else if (this->m_flags & 0x8) {
            UnpackAlphaShadowBitsFixed8888Mip1(static_cast<uint32_t*>(dest), x, pitch, size, alpha, layer, shadow);
        } else {
            UnpackAlphaShadowBitsFixed8888Mip0(static_cast<uint32_t*>(dest), x, pitch, size, alpha, layer, shadow);
        }

        return;
    }

    if (format != 3) {
        SErrDisplayAppFatal("CMapChunk::UnpackAlphaShadowBits(): Bad genformat.");
    }

    if (this->m_flags & 0x8) {
        UnpackAlphaShadowBitsUnfixed4444Mip1(static_cast<uint16_t*>(dest), x, pitch, size, alpha, shadow);
    } else {
        UnpackAlphaShadowBitsUnfixed4444Mip0(static_cast<uint16_t*>(dest), x, pitch, size, alpha, shadow);
    }
}

// OFFSET: 0x00767B80/0x14044E9A0
void CMapRenderChunk::UnpackAlphaBits(CMapChunk* chunk, uint32_t* dest, uint32_t size, SLayerAlpha* alpha, const uint8_t* shadow, EGxTexFormat format, int32_t fixedAlpha) {
    if (fixedAlpha) {
        if (format == 3) {
            if (this->m_flags & 0x8) {
                UnpackAlphaBitsFixed4444Mip1(reinterpret_cast<uint16_t*>(dest), size, alpha);
            } else {
                UnpackAlphaBitsFixed4444Mip0(reinterpret_cast<uint16_t*>(dest), size, alpha);
            }

            return;
        }

        if (format == 2) {
            if (alpha->m_alpha) {
                if (this->m_flags & 0x8) {
                    RecreateAlphaBitsFixed8888Mip1(dest, size, alpha, shadow);
                } else {
                    RecreateAlphaBitsFixed8888Mip0(dest, size, alpha, shadow);
                }
            } else {
                SLayerAlpha alphas[4];
                GatherLayerAlphas(chunk, alphas);

                if (this->m_flags & 0x8) {
                    UnpackAlphaBitsFixed8888Mip1(alphas, dest, size, shadow);
                } else {
                    UnpackAlphaBitsFixed8888Mip0(alphas, dest, size, shadow);
                }
            }

            return;
        }

        SErrDisplayAppFatal("CMapChunk::UnpackAlphaBits(): Bad genformat.");
    }

    if (format != 3) {
        SErrDisplayAppFatal("CMapChunk::UnpackAlphaBits(): Bad genformat.");
    }

    if (this->m_flags & 0x8) {
        UnpackAlphaBitsUnfixed4444Mip1(reinterpret_cast<uint16_t*>(dest), size, alpha);
    } else {
        UnpackAlphaBitsUnfixed4444Mip0(reinterpret_cast<uint16_t*>(dest), size, alpha);
    }
}
