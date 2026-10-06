#include "WoW/Source/DB/Rec/DungeonMapRec.h"

// STATIC: %0x00B46CE4
const DBCMeta DungeonMapRec::s_meta = { "DBFilesClient\\DungeonMap.dbc", 6, 32, DungeonMapRec::FixupStrings, 0 };

// PROVEN: 865a23a8 IDENTICAL/NONE
// OFFSET: 0x00401000
void DungeonMapRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void DungeonMapRec::ConvertRow(DungeonMapRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_floorIndex, &row[0x0008], sizeof(rec->m_floorIndex));
    memcpy(&rec->m_minX, &row[0x000C], sizeof(rec->m_minX));
    memcpy(&rec->m_minY, &row[0x0010], sizeof(rec->m_minY));
    memcpy(&rec->m_maxX, &row[0x0014], sizeof(rec->m_maxX));
    memcpy(&rec->m_maxY, &row[0x0018], sizeof(rec->m_maxY));
    memcpy(&rec->m_parentWorldMapID, &row[0x001C], sizeof(rec->m_parentWorldMapID));
}
#endif
