#ifndef WOW_SOURCE_GAME_GAMECLIENT_NAMECACHE_INL
#define WOW_SOURCE_GAME_GAMECLIENT_NAMECACHE_INL

// INLINED
inline NameCache::NameCache() {
    this->m_name[0] = '\0';
    this->m_declinedNames = NULL;
    this->m_realmName[0] = '\0';
    this->m_guid = WOWGUID();
}

// INLINED
inline NameCache::~NameCache() {
    delete this->m_declinedNames;
}

#endif
