#ifndef WOW_SOURCE_DB_REC_WEATHERREC_H
#define WOW_SOURCE_DB_REC_WEATHERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WeatherRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_ambienceID;
    /* 0008 */ int32_t m_effectType;
    /* 000C */ float m_transitionSkyBox;
    /* 0010 */ float m_effectColor [3];
    /* 001C */ DBCString m_effectTexture;

    static void FixupStrings(void* record, const char* strings);
};

#endif
