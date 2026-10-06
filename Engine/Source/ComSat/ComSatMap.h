#ifndef ENGINE_SOURCE_COMSAT_COMSATMAP_H
#define ENGINE_SOURCE_COMSAT_COMSATMAP_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/ComSat/ComSatHash.h"
#include "Engine/Source/ComSat/ComSatInterface.h"

namespace ComSat {

template <class T>
class MapKey {
    public:
    /* 0000 */ uint32_t m_hash;
    /* 0004 */ T m_value;

    MapKey();
    MapKey(const T& value);
    ~MapKey();
    bool operator==(const MapKey<T>& other) const;
};

template <class K, class V>
class Map {
    public:
    class Entry {
        public:
        /* 0000 */ MapKey<K> m_key;
        /* 0004 */ V m_value;

        Entry();
        Entry(const MapKey<K>& key);
        Entry(const MapKey<K>& key, const V& value);
    };

    /* 0000 */ Entry* m_entries;
    /* 0004 */ uint32_t m_used;
    /* 0008 */ uint32_t m_capacity;
    /* 000C */ Allocator* m_allocator;

    Map(Allocator& allocator);
    ~Map();
    void Clear();
    bool Find(const MapKey<K>& key, uint32_t& index) const;
    bool Get(const MapKey<K>& key, V& value) const;
    bool Insert(const MapKey<K>& key, const V& value);
    bool Next(uint32_t& iterator, MapKey<K>& key, V& value) const;
    uint32_t Reinsert(const Entry* entries, uint32_t count);
    bool Remove(const MapKey<K>& key, V& value);
    void Resize(uint32_t capacity);
    bool Shrink();
    V& operator[](const MapKey<K>& key);
};

}

// INLINED
template <class T>
ComSat::MapKey<T>::MapKey() {
    this->m_hash = 0;
}

// PROVEN: 7b6ecb44 IDENTICAL/NONE
// OFFSET: [0x00A34A20<unsigned __int64>]
template <class T>
ComSat::MapKey<T>::MapKey(const T& value) {
    this->m_hash = ComSat::Hash32(&value, sizeof(T), 0x811C9DC5) | 0x80000000;
    this->m_value = value;
}

// INLINED
template <class T>
ComSat::MapKey<T>::~MapKey() {
    if (this->m_hash) {
        this->m_hash = 0;
    }
}

// INLINED
template <class T>
bool ComSat::MapKey<T>::operator==(const ComSat::MapKey<T>& other) const {
    return this->m_hash == other.m_hash && this->m_value == other.m_value;
}

// INLINED
template <class K, class V>
ComSat::Map<K, V>::Entry::Entry() {
}

// INLINED
template <class K, class V>
ComSat::Map<K, V>::Entry::Entry(const ComSat::MapKey<K>& key) : m_key(key) {
}

// INLINED
template <class K, class V>
ComSat::Map<K, V>::Entry::Entry(const ComSat::MapKey<K>& key, const V& value) : m_key(key), m_value(value) {
}

// INLINED
template <class K, class V>
ComSat::Map<K, V>::Map(ComSat::Allocator& allocator) {
    this->m_entries = NULL;
    this->m_used = 0;
    this->m_capacity = 0;
    this->m_allocator = &allocator;
}

// INLINED
template <class K, class V>
ComSat::Map<K, V>::~Map() {
    this->Clear();
}

// OFFSET: [0x00A34BD0/0x1407E72F0<unsigned char, ComSat::Sound::Manager::LocalPlayer*>, %0x00975150<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>]
template <class K, class V>
void ComSat::Map<K, V>::Clear() {
    for (uint32_t i = 0; i < this->m_capacity; i++) {
        this->m_entries[i].~Entry();
    }
    this->m_used = 0;
    if (this->m_entries) {
        this->m_allocator->Free(this->m_entries);
        this->m_entries = NULL;
        this->m_capacity = 0;
    }
}

// OFFSET: [%0x00975220<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>, %0x009751A0<unsigned char, ComSat::Sound::Manager::LocalPlayer*>]
template <class K, class V>
bool ComSat::Map<K, V>::Find(const ComSat::MapKey<K>& key, uint32_t& index) const {
    if (!this->m_capacity) {
        return false;
    }
    uint32_t start = key.m_hash & (this->m_capacity - 1);
    index = start;
    if (!this->m_entries[start].m_key.m_hash) {
        return false;
    }
    do {
        if (this->m_entries[index].m_key == key) {
            return true;
        }
        index++;
        index &= this->m_capacity - 1;
    } while (index != start && this->m_entries[index].m_key.m_hash);
    return false;
}

// OFFSET: [%0x00975430<unsigned __int64, unsigned char>, %0x00975470<unsigned char, ComSat::Sound::Manager::LocalPlayer*>, %0x009754B0<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>]
template <class K, class V>
bool ComSat::Map<K, V>::Get(const ComSat::MapKey<K>& key, V& value) const {
    uint32_t index;
    if (!this->Find(key, index)) {
        return false;
    }
    value = this->m_entries[index].m_value;
    return true;
}

// OFFSET: [%0x00976FC0<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>, %0x0097AB70<unsigned char, ComSat::Sound::Manager::LocalPlayer*>]
template <class K, class V>
bool ComSat::Map<K, V>::Insert(const ComSat::MapKey<K>& key, const V& value) {
    if (this->m_used >= (3 * this->m_capacity) >> 2) {
        this->Resize(std::max(this->m_capacity * 2, 8u));
    }
    uint32_t index;
    if (this->Find(key, index)) {
        return false;
    }
    this->m_used++;
    new (&this->m_entries[index]) Entry(key, value);
    return true;
}

// OFFSET: [%0x00975020<unsigned __int64, unsigned char>, %0x00975090<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>, %0x0097A6E0<unsigned char, ComSat::Sound::Manager::LocalPlayer*>]
template <class K, class V>
bool ComSat::Map<K, V>::Next(uint32_t& iterator, ComSat::MapKey<K>& key, V& value) const {
    if (iterator >= this->m_capacity) {
        return false;
    }
    while (!this->m_entries[iterator].m_key.m_hash) {
        if (++iterator >= this->m_capacity) {
            return false;
        }
    }
    key = this->m_entries[iterator].m_key;
    value = this->m_entries[iterator].m_value;
    iterator++;
    return true;
}

// OFFSET: [#0x00881750<unsigned __int64, unsigned char>, #0x008817D0<unsigned __int64, float>, #0x00881850<unsigned char, ComSat::Sound::Manager::LocalPlayer*>, #0x008818D0<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>]
template <class K, class V>
uint32_t ComSat::Map<K, V>::Reinsert(const Entry* entries, uint32_t count) {
    uint32_t inserted = 0;
    for (uint32_t i = 0; i < count; i++) {
        if (entries->m_key.m_hash) {
            uint32_t index;
            this->Find(entries->m_key, index);
            new (&this->m_entries[index]) Entry(*entries);
            inserted++;
        }
        entries++;
    }
    return inserted;
}

// OFFSET: [%0x00977050<unsigned __int64, unsigned char>, %0x00977140<unsigned __int64, float>, %0x00977230<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>, %0x0097AC00<unsigned char, ComSat::Sound::Manager::LocalPlayer*>]
template <class K, class V>
bool ComSat::Map<K, V>::Remove(const ComSat::MapKey<K>& key, V& value) {
    uint32_t index;
    if (!this->Find(key, index)) {
        return false;
    }
    value = this->m_entries[index].m_value;
    this->m_entries[index].m_key.m_hash = 0;
    this->m_used--;
    if (!this->Shrink()) {
        for (uint32_t i = (index + 1) & (this->m_capacity - 1); this->m_entries[i].m_key.m_hash; i = (this->m_capacity - 1) & (i + 1)) {
            Entry* entry = &this->m_entries[i];
            uint32_t slot;
            if (!this->Find(entry->m_key, slot)) {
                new (&this->m_entries[slot]) Entry(*entry);
                this->m_entries[i].m_key.m_hash = 0;
            }
        }
    }
    return true;
}

// OFFSET: [%0x009754F0<unsigned __int64, unsigned char>, %0x00975580<unsigned __int64, float>, %0x0097A7B0<unsigned char, ComSat::Sound::Manager::LocalPlayer*>, %0x00975610<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>]
template <class K, class V>
void ComSat::Map<K, V>::Resize(uint32_t capacity) {
    uint32_t oldCapacity = this->m_capacity;
    Entry* oldEntries = this->m_entries;
    this->m_entries = static_cast<Entry*>(this->m_allocator->Alloc(sizeof(Entry) * capacity));
    this->m_capacity = capacity;
    for (uint32_t i = 0; i < capacity; i++) {
        new (&this->m_entries[i]) Entry();
    }
    if (oldEntries) {
        this->Reinsert(oldEntries, oldCapacity);
        for (uint32_t i = 0; i < oldCapacity; i++) {
            oldEntries[i].~Entry();
        }
        this->m_allocator->Free(oldEntries);
    }
}

// OFFSET: [%0x0097AB20<unsigned char, ComSat::Sound::Manager::LocalPlayer*>, %0x00976630<unsigned __int64, unsigned char>, %0x00976680<unsigned __int64, float>, %0x009765E0<unsigned __int64, ComSat::Sound::Manager::RemoteTalker*>]
template <class K, class V>
bool ComSat::Map<K, V>::Shrink() {
    if (this->m_used >= (3 * this->m_capacity) >> 3 || this->m_capacity <= 8) {
        return false;
    }
    this->Resize(std::max(this->m_capacity >> 1, 8u));
    return true;
}

// OFFSET: [%0x00976EA0<unsigned __int64, unsigned char>, %0x00976F30<unsigned __int64, float>]
template <class K, class V>
V& ComSat::Map<K, V>::operator[](const ComSat::MapKey<K>& key) {
    if (this->m_used >= (3 * this->m_capacity) >> 2) {
        this->Resize(std::max(this->m_capacity * 2, 8u));
    }
    uint32_t index;
    if (!this->Find(key, index)) {
        this->m_used++;
        new (&this->m_entries[index]) Entry(key);
    }
    return this->m_entries[index].m_value;
}

#endif
