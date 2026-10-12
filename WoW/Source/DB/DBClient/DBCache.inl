#include <cstring>
#include "WoW/Source/Game/GameClient/WardenClient.h"

// INLINED
inline uint32_t DBCacheHashKey(int32_t key) {
    return key;
}

// INLINED
inline uint32_t DBCacheHashKey(uint64_t key) {
    return static_cast<uint32_t>(key);
}

// INLINED
inline HASHKEY_INT::HASHKEY_INT() {
}

// INLINED
inline HASHKEY_INT::HASHKEY_INT(int32_t key) {
}

// INLINED
inline void DBCacheReadKey(CDataStore& msg, int32_t& key) {
    msg.Get(reinterpret_cast<uint32_t&>(key));
}

// INLINED
inline void DBCacheReadKey(CDataStore& msg, uint64_t& key) {
    msg.Get(key);
}

// INLINED
inline void DBCacheReadKey(CDataStore& msg, CWardenKey& key) {
    msg >> key;
}

// INLINED
inline void DBCacheWriteKey(CDataStore& msg, int32_t key) {
    msg.Put(static_cast<uint32_t>(key));
}

// INLINED
inline void DBCacheWriteKey(CDataStore& msg, uint64_t key) {
    msg.Put(key);
}

// INLINED
inline void DBCacheWriteKey(CDataStore& msg, const CWardenKey& key) {
    msg << key;
}

// INLINED
inline bool DBCacheIsNullKey(int32_t key) {
    return !key;
}

// INLINED
inline bool DBCacheIsNullKey(uint64_t key) {
    return !key;
}

// INLINED
inline bool DBCacheIsNullKey(const CWardenKey& key) {
    for (uint32_t i = 0; i < sizeof(key.m_key); i++) {
        if (key.m_key[i]) {
            return false;
        }
    }

    return true;
}

// INLINED
inline uint32_t DBCacheHashKey(const CWardenKey& key) {
    return key.Hash();
}

// INLINED
inline CWardenKey& CWardenKey::operator=(const CWardenKey& key) {
    memcpy(this->m_key, key.m_key, sizeof(this->m_key));
    return *this;
}
