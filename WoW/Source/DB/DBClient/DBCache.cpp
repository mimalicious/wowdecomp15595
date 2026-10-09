#include "DBCache.h"
#include <cstring>
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Game/GameClient/WardenClient.h"

// INLINED
template <>
CWardenKey DBCacheIntKey<CWardenKey>(int32_t key) {
    CWardenKey result;
    memset(result.m_key, 0, sizeof(result.m_key));
    return result;
}

// INLINED
HASHKEY_INT::HASHKEY_INT() {
}

// INLINED
HASHKEY_INT::HASHKEY_INT(int32_t key) {
}

// INLINED
bool HASHKEY_INT::operator==(const HASHKEY_INT& key) {
    return true;
}

// INLINED
void DBCacheReadKey(CDataStore& msg, int32_t& key) {
    msg.Get(reinterpret_cast<uint32_t&>(key));
}

// INLINED
void DBCacheReadKey(CDataStore& msg, uint64_t& key) {
    msg.Get(key);
}

// INLINED
void DBCacheReadKey(CDataStore& msg, CWardenKey& key) {
    msg >> key;
}

// INLINED
void DBCacheWriteKey(CDataStore& msg, int32_t key) {
    msg.Put(static_cast<uint32_t>(key));
}

// INLINED
void DBCacheWriteKey(CDataStore& msg, uint64_t key) {
    msg.Put(key);
}

// INLINED
void DBCacheWriteKey(CDataStore& msg, const CWardenKey& key) {
    msg << key;
}

// INLINED
bool DBCacheIsNullKey(int32_t key) {
    return !key;
}

// INLINED
bool DBCacheIsNullKey(uint64_t key) {
    return !key;
}

// INLINED
bool DBCacheIsNullKey(const CWardenKey& key) {
    for (uint32_t i = 0; i < sizeof(key.m_key); i++) {
        if (key.m_key[i]) {
            return false;
        }
    }

    return true;
}

// INLINED
uint32_t DBCacheHashKey(const CWardenKey& key) {
    return key.Hash();
}

// OFFSET: %0x004A20E0
CWardenKey& CWardenKey::operator=(const CWardenKey& key) {
    memcpy(this->m_key, key.m_key, sizeof(this->m_key));
    return *this;
}

// OFFSET: #0x00668750
bool CWardenKey::operator==(const CWardenKey& key) {
    return memcmp(this->m_key, key.m_key, sizeof(this->m_key)) == 0;
}

// OFFSET: %0x004A2190
uint32_t CWardenKey::Hash() const {
    return (this->m_key[12] << 24 | this->m_key[13] << 16 | this->m_key[14] << 8 | this->m_key[15]) & 0x7FFFFFFF;
}

// OFFSET: 0x0049B380
void GetCacheDirectory(char* path) {
    SStrCopy(path, "Cache/WDB/", 260);
    SStrPack(path, s_localeCode, 260);
    OsCreateDirectory(path, 1);
}
