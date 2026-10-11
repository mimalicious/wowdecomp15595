#include "WoW/Source/Battlenet/BSN/Protocol.h"

// OFFSET: 0x00665900
uint32_t BSN::Protocol::GetUInt32(const uint8_t*& cursor) {
    uint32_t value = 0;

    for (uint32_t i = 4; i; i--) {
        value = (value << 8) | *cursor++;
    }

    return value;
}

// OFFSET: 0x00665930/0x14030BE30
BSN::Protocol::Protocol::Protocol(const uint8_t* bytecode, uint32_t bytecodeSize, const void* const* codeInfo, uint32_t codeInfoCount) {
    this->m_bytecodeSize = bytecodeSize;
    this->m_codeInfoCount = codeInfoCount;
    this->m_codeInfo = codeInfo;
    this->m_bytecode = bytecode;

    const uint8_t* cursor = bytecode;

    uint32_t version = 0;
    for (uint32_t i = 4; i; i--) {
        version = (version << 8) | *cursor++;
    }
    this->m_version = version;

    uint32_t value = 0;
    for (uint32_t i = 4; i; i--) {
        value = (value << 8) | *cursor++;
    }
    this->unk_0014 = value;

    uint32_t typeIdCount = 0;
    for (uint32_t i = 4; i; i--) {
        typeIdCount = (typeIdCount << 8) | *cursor++;
    }
    this->m_typeIdCount = typeIdCount;

    uint32_t typeInfoSize = 0;
    for (uint32_t i = 4; i; i--) {
        typeInfoSize = (typeInfoSize << 8) | *cursor++;
    }
    this->m_typeInfoSize = typeInfoSize;

    uint32_t dumpInfoSize = 0;
    for (uint32_t i = 4; i; i--) {
        dumpInfoSize = (dumpInfoSize << 8) | *cursor++;
    }
    this->m_dumpInfoSize = dumpInfoSize;

    this->m_typeOffsets = cursor;
    const uint8_t* typeInfo = &cursor[4 * typeIdCount];
    this->m_typeInfo = typeInfo;

    const uint8_t* dumpOffsets = &typeInfo[typeInfoSize];

    if (dumpOffsets == &bytecode[bytecodeSize]) {
        this->m_dumpInfoSize = 0;
        this->m_dumpOffsets = NULL;
        this->m_dumpInfo = NULL;
    } else {
        this->m_dumpInfo = &dumpOffsets[4 * typeIdCount];
        this->m_dumpOffsets = dumpOffsets;
    }
}

// OFFSET: 0x00665A10/0x14030BF60
const uint8_t* BSN::Protocol::Protocol::TypeInfo(uint32_t typeId) const {
    const uint8_t* cursor = &this->m_typeOffsets[4 * typeId];
    uint32_t offset = 0;

    for (uint32_t i = 4; i; i--) {
        offset = (offset << 8) | *cursor++;
    }

    return &this->m_bytecode[offset];
}

// OFFSET: 0x00665A40
const uint8_t* BSN::Protocol::Protocol::DumpInfo(uint32_t typeId) const {
    if (!this->m_dumpInfoSize) {
        return NULL;
    }

    const uint8_t* cursor = &this->m_dumpOffsets[4 * typeId];
    uint32_t offset = BSN::Protocol::GetUInt32(cursor);

    if (!offset) {
        return NULL;
    }

    return &this->m_bytecode[offset];
}

// OFFSET: 0x00665A90/0x14030BFF0
const char* BSN::Protocol::Protocol::DumpString(const uint8_t* dumpInfo, uint32_t index) const {
    if (!this->m_dumpInfoSize || !dumpInfo) {
        return "";
    }

    const uint8_t* cursor = &dumpInfo[4 * index];
    uint32_t offset = BSN::Protocol::GetUInt32(cursor);

    if (!offset) {
        return NULL;
    }

    return reinterpret_cast<const char*>(&this->m_bytecode[offset]);
}
