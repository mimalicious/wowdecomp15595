#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"

// STATIC: 0x00C25B24/0x140A1B868
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C25B44/0x140A1B888
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C25B64/0x140A1B8A8
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 2, 0, 0 };

// STATIC: 0x00C25B84/0x140A1B8C8
const DBCMeta DungeonEncounterRec::s_meta = { "DBFilesClient\\DungeonEncounter.dbc", 8, 32, DungeonEncounterRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBF0/0x1407D84B0
void DungeonEncounterRec::FixupStrings(void* record, const char* strings) {
    DungeonEncounterRec* rec = static_cast<DungeonEncounterRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
