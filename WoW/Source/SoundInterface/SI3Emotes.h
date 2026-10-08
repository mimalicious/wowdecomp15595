#ifndef WOW_SOURCE_SOUNDINTERFACE_SI3EMOTES_H
#define WOW_SOURCE_SOUNDINTERFACE_SI3EMOTES_H

#include <cstddef>
#include <cstdint>

struct TEXTEMOTELOOKUP {
    /* 0000 */ int32_t m_soundIds[24][2];
    /* 00C0 */ int32_t m_emotesTextId;

    TEXTEMOTELOOKUP();
};

#endif
