#ifndef WOW_SOURCE_DB_REC_LIGHTREC_H
#define WOW_SOURCE_DB_REC_LIGHTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LightRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_gameCoords [3];
    /* 0014 */ float m_gameFalloffStart;
    /* 0018 */ float m_gameFalloffEnd;
    /* 001C */ int32_t m_lightParamsID [8];
};

#endif
