#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: cc89a821 IDENTICAL/DIFFERENT
// OFFSET: 0x00848E30/0x140576A30
CGxShader::~CGxShader() {
    SMemFree(this->m_data, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00848E70/0x140576A90
bool CGxShader::Load(uint8_t* data, uint32_t size) {
    this->m_data = data;

    if (size < sizeof(CGxShaderHeader)) {
        return false;
    }

    const CGxShaderHeader* header = reinterpret_cast<const CGxShaderHeader*>(data);

    if (header->m_magic != 'GXSH' || header->m_version != 0x10004) {
        return false;
    }

    if (header->m_permuteOffsetsOffset > size || header->m_blockOffsetsOffset > size || header->m_blockDataOffset > size) {
        return false;
    }

    this->m_permuteOffsets = reinterpret_cast<const uint32_t*>(data + header->m_permuteOffsetsOffset);
    this->m_blockOffsets = reinterpret_cast<const uint32_t*>(data + header->m_blockOffsetsOffset);
    this->m_blockData = data + header->m_blockDataOffset;
    this->m_permutes.SetCount(header->m_permuteCount);

    for (uint32_t i = 0; i < header->m_permuteCount; i++) {
        CGxShaderPermute& permute = this->m_permutes[i];
        permute.m_index = i;
        permute.m_apiSpecific = NULL;
        permute.m_shader = this;
        permute.m_valid = 0;
        permute.m_loaded = 0;
        permute.m_target = static_cast<uint8_t>(this->m_target);
    }

    return true;
}
