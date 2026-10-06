#include "WoW/Source/DB/Rec/DungeonMapChunkRec.h"

// STATIC: %0x00B46CB0
const DBCMeta DungeonMapChunkRec::s_meta = { "DBFilesClient\\DungeonMapChunk.dbc", 5, 20, DungeonMapChunkRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void DungeonMapChunkRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void DungeonMapChunkRec::ConvertRow(DungeonMapChunkRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_wmogroupID, &row[0x0008], sizeof(rec->m_wmogroupID));
    memcpy(&rec->m_dungeonMapID, &row[0x000C], sizeof(rec->m_dungeonMapID));
    memcpy(&rec->m_minZ, &row[0x0010], sizeof(rec->m_minZ));
}
#endif
