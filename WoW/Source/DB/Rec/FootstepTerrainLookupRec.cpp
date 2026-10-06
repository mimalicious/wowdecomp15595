#include "WoW/Source/DB/Rec/FootstepTerrainLookupRec.h"

// STATIC: %0x00B46A5C
const DBCMeta FootstepTerrainLookupRec::s_meta = { "DBFilesClient\\FootstepTerrainLookup.dbc", 5, 20, FootstepTerrainLookupRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void FootstepTerrainLookupRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void FootstepTerrainLookupRec::ConvertRow(FootstepTerrainLookupRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_creatureFootstepID, &row[0x0004], sizeof(rec->m_creatureFootstepID));
    memcpy(&rec->m_terrainSoundID, &row[0x0008], sizeof(rec->m_terrainSoundID));
    memcpy(&rec->m_soundID, &row[0x000C], sizeof(rec->m_soundID));
    memcpy(&rec->m_soundIdsplash, &row[0x0010], sizeof(rec->m_soundIdsplash));
}
#endif
