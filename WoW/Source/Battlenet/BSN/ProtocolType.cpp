#include "WoW/Source/Battlenet/BSN/Protocol.h"

// OFFSET: 0x00665AE0/0x14030C040
void BSN::Protocol::Type::Int::Get(int32_t intType, const uint8_t*& cursor, uint64_t& value) {
    uint32_t size = 1 << (intType >> 1);

    if ((intType & 1) || *cursor < 0x80) {
        value = 0;
    } else {
        value = static_cast<uint64_t>(-1);
    }

    for (; size; size--) {
        value = (value << 8) | *cursor++;
    }
}

// OFFSET: 0x00665B50
void BSN::Protocol::Type::Int::Get(int32_t intType, const uint8_t*& cursor, uint32_t& value) {
    uint32_t size = 1 << (intType >> 1);

    for (value = 0; size; cursor++) {
        value <<= 8;
        size--;
        value |= *cursor;
    }
}

// OFFSET: 0x00665B90/0x14030C090
void BSN::Protocol::Type::Int::GetSize(const uint8_t*& cursor, uint32_t& value) {
    uint8_t intType = *cursor++;
    BSN::Protocol::Type::Int::Get(intType, cursor, value);
}

// OFFSET: 0x00665BB0/0x14030C0E0
bool BSN::Protocol::Type::Bounds::Contains(uint64_t value) const {
    if (this->m_intType & 1) {
        return value >= this->m_min && value <= this->m_max;
    }

    return static_cast<int64_t>(value) >= static_cast<int64_t>(this->m_min) && static_cast<int64_t>(value) <= static_cast<int64_t>(this->m_max);
}

// OFFSET: 0x00665C00/0x14030C110
void BSN::Protocol::Type::Bounds::Get(bool fixed, const uint8_t*& cursor, Bounds& bounds) {
    uint8_t intType = *cursor++;
    bounds.m_intType = intType;

    if (fixed) {
        bounds.m_variable = false;
        bounds.m_bits = 0;
        BSN::Protocol::Type::Int::Get(intType, cursor, bounds.m_min);
        bounds.m_max = bounds.m_min;
    } else {
        bounds.m_variable = *cursor >> 7;
        bounds.m_bits = *cursor++ & 0x7F;
        BSN::Protocol::Type::Int::Get(bounds.m_intType, cursor, bounds.m_min);
        BSN::Protocol::Type::Int::Get(bounds.m_intType, cursor, bounds.m_max);
    }
}

// OFFSET: 0x00665C80/0x14030C260
void BSN::Protocol::Type::GetTypeId(const uint8_t*& cursor, uint32_t& typeId) {
    typeId = 0;

    for (uint32_t i = 4; i; i--) {
        typeId <<= 8;
        typeId |= *cursor++;
    }
}
