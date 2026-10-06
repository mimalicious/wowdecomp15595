#include "WoW/Source/DB/Rec/WorldMapContinentRec.h"

// STATIC: %0x00B44514
const DBCMeta WorldMapContinentRec::s_meta = { "DBFilesClient\\WorldMapContinent.dbc", 11, 56, WorldMapContinentRec::FixupStrings, 0 };

// PROVEN: fde269c0 IDENTICAL
// OFFSET: 0x00401000
void WorldMapContinentRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void WorldMapContinentRec::ConvertRow(WorldMapContinentRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_leftBoundary, &row[0x0008], sizeof(rec->m_leftBoundary));
    memcpy(&rec->m_rightBoundary, &row[0x000C], sizeof(rec->m_rightBoundary));
    memcpy(&rec->m_topBoundary, &row[0x0010], sizeof(rec->m_topBoundary));
    memcpy(&rec->m_bottomBoundary, &row[0x0014], sizeof(rec->m_bottomBoundary));
    memcpy(rec->m_continentOffset, &row[0x0018], sizeof(rec->m_continentOffset));
    memcpy(&rec->m_scale, &row[0x0020], sizeof(rec->m_scale));
    memcpy(rec->m_taxiMin, &row[0x0024], sizeof(rec->m_taxiMin));
    memcpy(rec->m_taxiMax, &row[0x002C], sizeof(rec->m_taxiMax));
    memcpy(&rec->m_worldMapID, &row[0x0034], sizeof(rec->m_worldMapID));
}
#endif
