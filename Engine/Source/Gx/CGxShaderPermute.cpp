#include <cstdint>
#include <cstring>
#include "Engine/Source/Gx/CGxShaderPermute.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Storm/H/storm.h"
#include "ThirdParty/zlib/zlib.h"

// PROVEN: c10ec8e4 IDENTICAL/IDENTICAL
// OFFSET: 0x00848B80/0x1405766C0
int32_t CGxShaderPermute::Valid() {
    if (!this->m_loaded) {
        g_theGxDevicePtr->IShaderCreate(this);
    }

    return this->m_valid;
}

// OFFSET: %0x007AE5D0
uint8_t* CGxShaderPermute::Load() {
    CGxShader* shader = this->m_shader;
    uint32_t start = shader->m_permuteOffsets[this->m_index];
    uint32_t size = shader->m_permuteOffsets[this->m_index + 1] - start;
    uint8_t* code = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
    uint32_t block = start >> 14;
    uint32_t blockOffset = start - (block << 14);
    uint8_t* dest = code;

    if (blockOffset) {
        uint32_t compressedOffset = shader->m_blockOffsets[block];
        uint32_t compressedSize = shader->m_blockOffsets[block + 1] - compressedOffset;
        uLongf blockSize = blockOffset + size >= 0x4000 ? 0x4000 : blockOffset + size;
        const uint8_t* source = shader->m_blockData + compressedOffset;
        uint32_t copied;

        if (compressedSize == 0x4000) {
            copied = blockSize - blockOffset;
            memcpy(code, source + blockOffset, copied);
        } else {
            uint8_t buffer[0x4000];
            uncompress(buffer, &blockSize, source, compressedSize);
            copied = blockSize - blockOffset;
            memcpy(code, buffer + blockOffset, copied);
        }

        dest = code + copied;
        size += blockOffset - blockSize;
        block++;
    }

    while (size) {
        uint32_t compressedOffset = shader->m_blockOffsets[block];
        uint32_t compressedSize = shader->m_blockOffsets[block + 1] - compressedOffset;
        uLongf blockSize = size >= 0x4000 ? 0x4000 : size;
        const uint8_t* source = shader->m_blockData + compressedOffset;

        if (compressedSize == 0x4000) {
            memcpy(dest, source, 0x4000);
        } else {
            uncompress(dest, &blockSize, source, compressedSize);
        }

        dest += blockSize;
        size -= blockSize;
        block++;
    }

    const uint32_t* header = reinterpret_cast<const uint32_t*>(code);
    this->unk_000C = header[0];
    this->unk_0010 = header[1];
    this->unk_0014 = reinterpret_cast<const uint16_t*>(code)[4];
    this->unk_0016[0] = header[3];
    this->unk_0016[1] = header[4];
    this->unk_0016[2] = header[5];
    this->unk_0016[3] = header[6];

    return code;
}
