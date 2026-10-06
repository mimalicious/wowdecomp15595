#include "WoW/Source/DB/Rec/FactionGroupRec.h"

// STATIC: 0x00C1E438/0x140A12660
const DBCMeta FactionGroupRec::s_meta = { "DBFilesClient\\FactionGroup.dbc", 4, 16, FactionGroupRec::FixupStrings, 0 };

// PROVEN: 6ff3adc3 IDENTICAL/NONE
// OFFSET: 0x00A2EC50/0x1407D8590
void FactionGroupRec::FixupStrings(void* record, const char* strings) {
    FactionGroupRec* rec = static_cast<FactionGroupRec*>(record);
    DBCFixupString(rec->m_internalName, strings);
    DBCFixupString(rec->m_name, strings);
}
