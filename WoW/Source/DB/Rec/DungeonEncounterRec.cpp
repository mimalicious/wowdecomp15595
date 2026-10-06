#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"

// STATIC: 0x00C25B84/0x140A1B8C8
const DBCMeta DungeonEncounterRec::s_meta = { "DBFilesClient\\DungeonEncounter.dbc", 8, 32, DungeonEncounterRec::FixupStrings, 0 };

// PROVEN: 303b32ea IDENTICAL/NONE
// OFFSET: 0x00A2EBF0/0x1407D84B0
void DungeonEncounterRec::FixupStrings(void* record, const char* strings) {
    DungeonEncounterRec* rec = static_cast<DungeonEncounterRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
