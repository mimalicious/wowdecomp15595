#include "WoW/Source/DB/Rec/MapDifficultyRec.h"

// STATIC: %0x00B45C58
const DBCMeta MapDifficultyRec::s_meta = { "DBFilesClient\\MapDifficulty.dbc", 7, 28, MapDifficultyRec::FixupStrings, 0 };

// OFFSET: %0x0097C320
void MapDifficultyRec::FixupStrings(void* record, const char* strings) {
    MapDifficultyRec* rec = static_cast<MapDifficultyRec*>(record);
    rec->m_message = &strings[reinterpret_cast<uintptr_t>(rec->m_message)];
    rec->m_difficultystring = &strings[reinterpret_cast<uintptr_t>(rec->m_difficultystring)];
}

#if defined(WOWDECOMP_ARCH_64)
void MapDifficultyRec::ConvertRow(MapDifficultyRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_difficulty, &row[0x0008], sizeof(rec->m_difficulty));
    rec->m_message = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    memcpy(&rec->m_raidDuration, &row[0x0010], sizeof(rec->m_raidDuration));
    memcpy(&rec->m_maxPlayers, &row[0x0014], sizeof(rec->m_maxPlayers));
    rec->m_difficultystring = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
}
#endif
