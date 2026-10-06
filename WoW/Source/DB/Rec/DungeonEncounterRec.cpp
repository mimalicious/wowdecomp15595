#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"

// STATIC: %0x00B46D1C
const DBCMeta DungeonEncounterRec::s_meta = { "DBFilesClient\\DungeonEncounter.dbc", 8, 32, DungeonEncounterRec::FixupStrings, 0 };

// OFFSET: %0x0097C410
void DungeonEncounterRec::FixupStrings(void* record, const char* strings) {
    DungeonEncounterRec* rec = static_cast<DungeonEncounterRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void DungeonEncounterRec::ConvertRow(DungeonEncounterRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_difficulty, &row[0x0008], sizeof(rec->m_difficulty));
    memcpy(&rec->m_orderIndex, &row[0x000C], sizeof(rec->m_orderIndex));
    memcpy(&rec->m_bit, &row[0x0010], sizeof(rec->m_bit));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
}
#endif
