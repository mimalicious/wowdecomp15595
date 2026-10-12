#include "WoW/Source/Battlenet/BSN/Protocol.h"

// PROVEN: 95719e48 IDENTICAL/IDENTICAL
// OFFSET: 0x00663BD0/0x140309D20
uint32_t BSN::Protocol::Meta::PeekHeaderEnum(const Protocol* protocol, uint32_t typeId) {
    return *protocol->TypeInfo(typeId) & 0x3F;
}

// OFFSET: 0x00663BF0/0x140309D40
BSN::Protocol::Meta::Base::Base(const Protocol* protocol, uint32_t typeId) {
    this->m_protocol = protocol;
    this->m_typeId = typeId;
    this->m_typeInfo = protocol->TypeInfo(typeId);
    this->m_dumpInfo = protocol->DumpInfo(typeId);

    const void* const* codeInfo = protocol->m_codeInfo;
    uintptr_t index = reinterpret_cast<uintptr_t>(codeInfo[typeId + 1]);
    this->m_codeInfo = index ? &codeInfo[index] : NULL;

    this->m_header = *this->m_typeInfo++;
}

// PROVEN: db79175d IDENTICAL/DIFFERENT
// OFFSET: 0x00663C50/0x140309DC0
BSN::Protocol::Meta::Array::Array(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(this->m_header >> 7, this->m_typeInfo, this->m_bounds);
    BSN::Protocol::Type::GetTypeId(this->m_typeInfo, this->m_elementTypeId);
}

// PROVEN: 21c8f97d IDENTICAL/IDENTICAL
// OFFSET: 0x00663CD0/0x140309E70
BSN::Protocol::Meta::BitArray::BitArray(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(this->m_header >> 7, this->m_typeInfo, this->m_bounds);
}

// PROVEN: f01802c5 IDENTICAL/IDENTICAL
// OFFSET: 0x00663CD0/0x140309E70
BSN::Protocol::Meta::Blob::Blob(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(this->m_header >> 7, this->m_typeInfo, this->m_bounds);
}

// OFFSET: 0x00663D10/0x140309EA0
bool BSN::Protocol::Meta::Choice::GetTagField(uint64_t tag, uint32_t& field) const {
    if (!(this->m_header & 0x80)) {
        uint32_t size = 1 << (this->m_bounds.m_intType >> 1);
        field = 0;

        if (this->m_count) {
            do {
                const uint8_t* cursor = &this->m_typeInfo[size * field];
                uint64_t value;
                BSN::Protocol::Type::Int::Get(this->m_bounds.m_intType, cursor, value);

                if (tag == value) {
                    return true;
                }
            } while (++field < this->m_count);
        }
    } else if (this->m_bounds.Contains(tag)) {
        field = static_cast<uint32_t>(tag - this->m_bounds.m_min);
        return true;
    }

    return false;
}

// OFFSET: 0x00663DB0/0x140309F50
bool BSN::Protocol::Meta::Choice::GetFieldTag(uint32_t field, uint64_t& tag) const {
    if (!(this->m_header & 0x80)) {
        if (field < this->m_count) {
            const uint8_t* cursor = &this->m_typeInfo[field * (1 << (this->m_bounds.m_intType >> 1))];
            BSN::Protocol::Type::Int::Get(this->m_bounds.m_intType, cursor, tag);
            return true;
        }
    } else {
        tag = field + this->m_bounds.m_min;

        if (this->m_bounds.Contains(tag)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00663E30/0x140309FC0
bool BSN::Protocol::Meta::Choice::GetFieldTypeId(uint32_t field, uint32_t& typeId) const {
    if (field >= this->m_count) {
        return false;
    }

    const uint8_t* cursor;

    if (!(this->m_header & 0x80)) {
        cursor = &this->m_typeInfo[this->m_count * (1 << (this->m_bounds.m_intType >> 1)) + 4 * field];
    } else {
        cursor = &this->m_typeInfo[4 * field];
    }

    BSN::Protocol::Type::GetTypeId(cursor, typeId);
    return true;
}

// OFFSET: 0x00663F00/0x14030A040
bool BSN::Protocol::Meta::Enum::GetEnumField(uint64_t value, uint32_t& field) const {
    if (!(this->m_header & 0x80)) {
        const uint8_t* cursor = this->m_typeInfo;
        field = 0;

        if (this->m_count) {
            do {
                uint64_t entry;
                BSN::Protocol::Type::Int::Get(this->m_bounds.m_intType, cursor, entry);

                if (value == entry) {
                    return true;
                }
            } while (++field < this->m_count);
        }
    } else if (this->m_bounds.Contains(value)) {
        field = static_cast<uint32_t>(value - this->m_bounds.m_min);
        return true;
    }

    return false;
}

// PROVEN: 1dd3db52 IDENTICAL/IDENTICAL
// OFFSET: 0x00663F90/0x140309E10
BSN::Protocol::Meta::Int::Int(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);
}

// PROVEN: 2a91749a IDENTICAL/IDENTICAL
// OFFSET: 0x00663F90/0x140309E10
BSN::Protocol::Meta::AsciiString::AsciiString(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);
}

// PROVEN: c0926f36 IDENTICAL/IDENTICAL
// OFFSET: 0x00663FE0/0x14030A250
BSN::Protocol::Meta::User::User(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::GetTypeId(this->m_typeInfo, this->m_userTypeId);
}

// PROVEN: 2c26412a IDENTICAL/IDENTICAL
// OFFSET: 0x00663FE0/0x14030A250
BSN::Protocol::Meta::Optional::Optional(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::GetTypeId(this->m_typeInfo, this->m_elementTypeId);
}

// PROVEN: c0b3cda7 IDENTICAL/IDENTICAL
// OFFSET: 0x00663FC0/0x14030A0E0
BSN::Protocol::Meta::Bool::Bool(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// PROVEN: 0f1b830e IDENTICAL/IDENTICAL
// OFFSET: 0x00663FC0/0x14030A0E0
BSN::Protocol::Meta::FourCC::FourCC(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// PROVEN: 4ef8154b IDENTICAL/IDENTICAL
// OFFSET: 0x00663FC0/0x14030A0E0
BSN::Protocol::Meta::Null::Null(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// PROVEN: 5a46550a IDENTICAL/IDENTICAL
// OFFSET: 0x00663FC0/0x14030A0E0
BSN::Protocol::Meta::Real32::Real32(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// PROVEN: 01c1ae32 IDENTICAL/IDENTICAL
// OFFSET: 0x00663FC0/0x14030A0E0
BSN::Protocol::Meta::Real64::Real64(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
}

// OFFSET: 0x00663EA0/0x14030A100
BSN::Protocol::Meta::Enum::Enum(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);

    if (!(this->m_header & 0x80)) {
        BSN::Protocol::Type::Int::GetSize(this->m_typeInfo, this->m_count);
    } else {
        this->m_count = static_cast<uint32_t>(this->m_bounds.m_max) - static_cast<uint32_t>(this->m_bounds.m_min) + 1;
    }
}

// OFFSET: 0x00663EA0/0x14030A100
BSN::Protocol::Meta::Choice::Choice(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);

    if (!(this->m_header & 0x80)) {
        BSN::Protocol::Type::Int::GetSize(this->m_typeInfo, this->m_count);
    } else {
        this->m_count = static_cast<uint32_t>(this->m_bounds.m_max) - static_cast<uint32_t>(this->m_bounds.m_min) + 1;
    }
}

// OFFSET: 0x00663EA0/0x14030A100
BSN::Protocol::Meta::Struct::Struct(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_bounds);

    if (!(this->m_header & 0x80)) {
        BSN::Protocol::Type::Int::GetSize(this->m_typeInfo, this->m_count);
    } else {
        this->m_count = static_cast<uint32_t>(this->m_bounds.m_max) - static_cast<uint32_t>(this->m_bounds.m_min) + 1;
    }
}

// OFFSET: 0x00664010/0x14030A160
bool BSN::Protocol::Meta::Struct::GetMember(uint32_t index, uint32_t& offset, uint32_t& typeId) const {
    if (index >= this->m_count) {
        return false;
    }

    offset = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(this->m_codeInfo[index + 1]));

    const uint8_t* cursor;

    if (!(this->m_header & 0x80)) {
        cursor = &this->m_typeInfo[4 * index + this->m_count * (1 << (this->m_bounds.m_intType >> 1))];
    } else {
        cursor = &this->m_typeInfo[4 * index];
    }

    BSN::Protocol::Type::GetTypeId(cursor, typeId);
    return true;
}

// PROVEN: a05820e6 IDENTICAL/DIFFERENT
// OFFSET: 0x00664090/0x14030A1E0
BSN::Protocol::Meta::String::String(const Protocol* protocol, uint32_t typeId) : Base(protocol, typeId) {
    BSN::Protocol::Type::Bounds::Get(false, this->m_typeInfo, this->m_charBounds);
    this->m_bounds = this->m_charBounds;
    this->m_bounds.m_bits += 2;
    this->m_bounds.m_max *= 4;
}

// OFFSET: 0x006640F0/0x14030A280
bool BSN::Protocol::Meta::String::IsValid(uint32_t length, const uint8_t* data, uint32_t& count) {
    uint32_t position = 0;
    count = 0;

    if (!length) {
        return true;
    }

    do {
        const uint8_t* sequence = &data[position];
        uint32_t index = 0;

        while (*sequence < BSN::s_utf8Sequences[index].m_ranges[0][0] || *sequence > BSN::s_utf8Sequences[index].m_ranges[0][1]) {
            if (++index >= 9) {
                return false;
            }
        }

        uint32_t sequenceLength = BSN::s_utf8Sequences[index].m_length;

        if (length - position < sequenceLength) {
            return false;
        }

        for (uint32_t i = 1; i < sequenceLength; i++) {
            if (sequence[i] < BSN::s_utf8Sequences[index].m_ranges[i][0] || sequence[i] > BSN::s_utf8Sequences[index].m_ranges[i][1]) {
                return false;
            }
        }

        position += sequenceLength;
        count++;
    } while (position < length);

    return true;
}
