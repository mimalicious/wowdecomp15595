#include "WoW/Source/DB/Rec/FactionRec.h"

// STATIC: 0x00C2562C/0x140A1B210
const DBCMeta FactionRec::s_meta = { "DBFilesClient\\Faction.dbc", 12, 104, FactionRec::FixupStrings, 0 };

// PROVEN: 492a6bd0 IDENTICAL/NONE
// OFFSET: 0x00A2EBD0/0x1407D8470
void FactionRec::FixupStrings(void* record, const char* strings) {
    FactionRec* rec = static_cast<FactionRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
