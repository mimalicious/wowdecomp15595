#include "WoW/Source/Battlenet/BattlenetMap.h"

// OFFSET: 0x006607C0
Battlenet::String::String(const char* string) {
    if (!string) {
        this->m_block = NULL;
        this->m_string = NULL;
        return;
    }

    uint32_t capacity = (static_cast<uint32_t>(strlen(string)) + 40) & ~31u;
    Block* block = static_cast<Block*>(Battlenet::g_allocator->Alloc(capacity + sizeof(Block), "d:\\buildserver\\wow\\4\\work\\wow-code\\branches\\wow-patch-4_3_4-branch\\wow\\source\\battlenet\\tumor\\String.cpp(125):Data[capacity]"));

    if (block) {
        block->m_refCount = 1;
        block->m_capacity = capacity;
        Battlenet::StringCopy(block + 1, string, capacity);
    }

    this->m_block = block;
    this->m_string = reinterpret_cast<const char*>(block + 1);
}

// OFFSET: 0x00660B80
Battlenet::MapKey::MapKey(const String& value) {
    const char* string = value.GetString();
    uint32_t hash = 0x811C9DC5;

    for (uint8_t c = *string; c; c = *++string) {
        hash = 16777619 * (c ^ hash);
    }

    this->m_hash = hash | 0x80000000;
    this->m_value.m_block = value.m_block;
    this->m_value.m_string = value.m_string;

    if (value.m_block) {
        value.m_block->m_refCount++;
    }
}

// OFFSET: 0x00660BE0
Battlenet::MapKey::MapKey(const char* value) {
    String key(value);
    const char* string = key.GetString();
    uint32_t hash = 0x811C9DC5;

    for (uint8_t c = *string; c; c = *++string) {
        hash = 16777619 * (c ^ hash);
    }

    this->m_hash = hash | 0x80000000;
    this->m_value.m_block = key.m_block;
    this->m_value.m_string = key.m_string;

    if (key.m_block) {
        key.m_block->m_refCount++;
    }
}

// OFFSET: 0x00660C30
Battlenet::MapKey::~MapKey() {
    this->m_hash = 0;
}

// OFFSET: 0x00660C70
Battlenet::MapKey& Battlenet::MapKey::operator=(const MapKey& other) {
    this->m_hash = other.m_hash;
    this->m_value = other.m_value;
    return *this;
}
