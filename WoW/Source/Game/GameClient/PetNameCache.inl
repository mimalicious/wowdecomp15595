#ifndef WOW_SOURCE_GAME_GAMECLIENT_PETNAMECACHE_INL
#define WOW_SOURCE_GAME_GAMECLIENT_PETNAMECACHE_INL

// INLINED
inline PetNameCache::PetNameCache() {
    this->m_petNumber = 0;
    this->m_name[0] = '\0';
    this->m_declinedNames = NULL;
    this->m_timestamp = 0;
}

// INLINED
inline PetNameCache::~PetNameCache() {
    delete this->m_declinedNames;
}

#endif
