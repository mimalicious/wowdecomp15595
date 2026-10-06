#include "WoW/Source/DB/Rec/AreaTableRec.h"

// STATIC: 0x00C285B4/0x140A1EDA0
const DBCMeta AreaTableRec::s_meta = { "DBFilesClient\\AreaTable.dbc", 23, 104, AreaTableRec::FixupStrings, 0 };

// PROVEN: 9a6652b1 IDENTICAL/NONE
// OFFSET: 0x00A2EC40/0x1407D85D0
void AreaTableRec::FixupStrings(void* record, const char* strings) {
    AreaTableRec* rec = static_cast<AreaTableRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}
