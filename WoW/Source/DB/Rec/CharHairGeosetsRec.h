#ifndef WOW_SOURCE_DB_REC_CHARHAIRGEOSETSREC_H
#define WOW_SOURCE_DB_REC_CHARHAIRGEOSETSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CharHairGeosetsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_raceID;
    /* 0008 */ int32_t m_sexID;
    /* 000C */ int32_t m_variationID;
    /* 0010 */ int32_t m_geosetID;
    /* 0014 */ int32_t m_showscalp;
};

#endif
