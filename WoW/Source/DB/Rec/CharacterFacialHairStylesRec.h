#ifndef WOW_SOURCE_DB_REC_CHARACTERFACIALHAIRSTYLESREC_H
#define WOW_SOURCE_DB_REC_CHARACTERFACIALHAIRSTYLESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CharacterFacialHairStylesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_raceID;
    /* 0008 */ int32_t m_sexID;
    /* 000C */ int32_t m_variationID;
    /* 0010 */ int32_t m_geoset [5];

    static void FixupStrings(void* record, const char* strings);
};

#endif
