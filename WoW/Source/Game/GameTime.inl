#ifndef WOW_SOURCE_GAME_GAMETIME_INL
#define WOW_SOURCE_GAME_GAMETIME_INL

// INLINED
inline TIMESTAMPSTRUCT::~TIMESTAMPSTRUCT() {
    this->m_callbacks.Clear();
}

#endif
