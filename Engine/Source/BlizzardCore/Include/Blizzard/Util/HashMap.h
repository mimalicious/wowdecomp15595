#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_HASHMAP_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_HASHMAP_H

#include <algorithm>
#include <cstdint>
#include "Engine/Source/Storm/H/storm.h"

namespace Blizzard {
namespace Util {

template <class K, class V>
class HashMap {
    public:
    struct Entry {
        /* 0000 */ K m_key;
        /* 0008 */ V m_value;
    };

    /* 0000 */ Entry* m_entries;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ uint32_t m_capacity;

    ~HashMap();
    uint32_t CopyEntries(const Entry* entries, uint32_t count);
    bool Find(const K& key, uint32_t* index);
    bool Get(const K& key, V* value);
    V* Insert(const K& key);
    bool Remove(const K& key, V* value);
    void Resize(uint32_t capacity);
    bool Shrink();
};

}
}

// INLINED
template <class K, class V>
Blizzard::Util::HashMap<K, V>::~HashMap() {
    for (uint32_t i = 0; i < this->m_capacity; i++) {
        if (this->m_entries[i].m_key.m_lo) {
            this->m_entries[i].m_key.m_lo = 0;
            this->m_entries[i].m_key.m_hi = 0;
        }
    }

    this->m_count = 0;

    if (this->m_entries) {
        SMemFree(this->m_entries, __FILE__, __LINE__, 0);
        this->m_entries = NULL;
        this->m_capacity = 0;
    }
}

// OFFSET: 0x007BD960
template <class K, class V>
bool Blizzard::Util::HashMap<K, V>::Find(const K& key, uint32_t* index) {
    if (!this->m_capacity) {
        return false;
    }

    for (*index = key.m_lo & (this->m_capacity - 1); this->m_entries[*index].m_key.m_lo; *index = (*index + 1) & (this->m_capacity - 1)) {
        if (this->m_entries[*index].m_key.m_lo == key.m_lo && this->m_entries[*index].m_key.m_hi == key.m_hi) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x007C0CE0
template <class K, class V>
bool Blizzard::Util::HashMap<K, V>::Get(const K& key, V* value) {
    uint32_t index;

    if (!this->Find(key, &index)) {
        return false;
    }

    *value = this->m_entries[index].m_value;

    return true;
}

// OFFSET: 0x007BDE50<System_SFile::SignatureKey, Blizzard::Util::Digest<16>>
template <class K, class V>
uint32_t Blizzard::Util::HashMap<K, V>::CopyEntries(const Entry* entries, uint32_t count) {
    uint32_t copied = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].m_key.m_lo) {
            uint32_t index = 0;
            this->Find(entries[i].m_key, &index);
            this->m_entries[index] = entries[i];
            copied++;
        }
    }

    return copied;
}

// OFFSET: 0x007C68D0<System_SFile::SignatureKey, Blizzard::Util::Digest<16>>
template <class K, class V>
V* Blizzard::Util::HashMap<K, V>::Insert(const K& key) {
    if (this->m_count >= 3 * this->m_capacity / 4) {
        this->Resize(std::max(2 * this->m_capacity, 8u));
    }

    uint32_t index = 0;

    if (!this->Find(key, &index)) {
        this->m_count++;
        this->m_entries[index].m_key = key;
    }

    return &this->m_entries[index].m_value;
}

// OFFSET: 0x007C1090<System_SFile::SignatureKey, Blizzard::Util::Digest<16>>
template <class K, class V>
void Blizzard::Util::HashMap<K, V>::Resize(uint32_t capacity) {
    Entry* old = this->m_entries;
    uint32_t oldCapacity = this->m_capacity;

    this->m_entries = static_cast<Entry*>(SMemAlloc(sizeof(Entry) * capacity, __FILE__, __LINE__, 0));
    this->m_capacity = capacity;

    for (uint32_t i = 0; i < capacity; i++) {
        this->m_entries[i].m_key.m_lo = 0;
        this->m_entries[i].m_key.m_hi = 0;
    }

    if (!old) {
        return;
    }

    this->CopyEntries(old, oldCapacity);

    for (uint32_t i = 0; i < oldCapacity; i++) {
        if (old[i].m_key.m_lo) {
            old[i].m_key.m_lo = 0;
            old[i].m_key.m_hi = 0;
        }
    }

    SMemFree(old, __FILE__, __LINE__, 0);
}

// OFFSET: 0x007C6EC0<System_SFile::SignatureKey, Blizzard::Util::Digest<16>>
template <class K, class V>
bool Blizzard::Util::HashMap<K, V>::Remove(const K& key, V* value) {
    uint32_t index;

    if (!this->Find(key, &index)) {
        return false;
    }

    *value = this->m_entries[index].m_value;
    this->m_entries[index].m_key.m_lo = 0;
    this->m_entries[index].m_key.m_hi = 0;
    this->m_count--;

    if (this->Shrink()) {
        return true;
    }

    while (true) {
        index = (index + 1) & (this->m_capacity - 1);
        Entry* entry = &this->m_entries[index];

        if (!entry->m_key.m_lo && !entry->m_key.m_hi) {
            break;
        }

        uint32_t target = 0;

        if (!this->Find(entry->m_key, &target)) {
            this->m_entries[target] = *entry;
            this->m_entries[index].m_key.m_lo = 0;
            this->m_entries[index].m_key.m_hi = 0;
        }
    }

    return true;
}

// OFFSET: 0x007C59C0/0x1404C0B00<System_SFile::SignatureKey, Blizzard::Util::Digest<16>>
template <class K, class V>
bool Blizzard::Util::HashMap<K, V>::Shrink() {
    int32_t capacity = this->m_capacity;

    if (static_cast<int32_t>(this->m_count) >= 3 * capacity / 8 || capacity <= 8) {
        return false;
    }

    this->Resize(std::max(capacity / 2, 8));

    return true;
}

#endif
