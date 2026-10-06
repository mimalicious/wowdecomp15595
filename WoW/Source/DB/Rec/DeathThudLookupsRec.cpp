#include "WoW/Source/DB/Rec/DeathThudLookupsRec.h"

// STATIC: %0x00B46D90
const DBCMeta DeathThudLookupsRec::s_meta = { "DBFilesClient\\DeathThudLookups.dbc", 5, 20, DeathThudLookupsRec::FixupStrings, 0 };

// PROVEN: d0a3cad0 IDENTICAL
// OFFSET: 0x00401000
void DeathThudLookupsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void DeathThudLookupsRec::ConvertRow(DeathThudLookupsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_sizeClass, &row[0x0004], sizeof(rec->m_sizeClass));
    memcpy(&rec->m_terrainTypeSoundID, &row[0x0008], sizeof(rec->m_terrainTypeSoundID));
    memcpy(&rec->m_soundEntryID, &row[0x000C], sizeof(rec->m_soundEntryID));
    memcpy(&rec->m_soundEntryIdwater, &row[0x0010], sizeof(rec->m_soundEntryIdwater));
}
#endif
