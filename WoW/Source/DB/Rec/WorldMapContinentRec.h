#ifndef WOW_SOURCE_DB_REC_WORLDMAPCONTINENTREC_H
#define WOW_SOURCE_DB_REC_WORLDMAPCONTINENTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class WorldMapContinentRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_leftBoundary;
    /* 000C */ int32_t m_rightBoundary;
    /* 0010 */ int32_t m_topBoundary;
    /* 0014 */ int32_t m_bottomBoundary;
    /* 0018 */ float m_continentOffset [2];
    /* 0020 */ float m_scale;
    /* 0024 */ float m_taxiMin [2];
    /* 002C */ float m_taxiMax [2];
    /* 0034 */ int32_t m_worldMapID;

    static void FixupStrings(void* record, const char* strings);
};

#endif
