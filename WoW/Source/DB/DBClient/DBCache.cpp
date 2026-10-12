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
bool HASHKEY_INT::operator==(const HASHKEY_INT& key) {
    return true;
}

// OFFSET: 0x0049B240/0x14001B6D0
bool CWardenKey::operator==(const CWardenKey& key) {
    return memcmp(this->m_key, key.m_key, sizeof(this->m_key)) == 0;
}

// OFFSET: 0x0049B2C0
uint32_t CWardenKey::Hash() const {
    return (this->m_key[12] << 24 | this->m_key[13] << 16 | this->m_key[14] << 8 | this->m_key[15]) & 0x7FFFFFFF;
}

// PROVEN: e9363243 DIFFERENT/IDENTICAL
// OFFSET: 0x0049B380/0x1400C7080
void GetCacheDirectory(char* path) {
    SStrCopy(path, "Cache/WDB/", 260);
    SStrPack(path, s_localeCode, 260);
    OsCreateDirectory(path, 1);
}
