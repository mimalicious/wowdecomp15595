#include "WoW/Source/DB/Rec/BattlemasterListRec.h"

// STATIC: 0x00C26814/0x140A1C7E0
const DBCMeta BattlemasterListRec::s_meta = { "DBFilesClient\\BattlemasterList.dbc", 13, 80, BattlemasterListRec::FixupStrings, 0 };

// PROVEN: aa27b905 IDENTICAL/NONE
// OFFSET: 0x00A2EC40/0x1407D85D0
void BattlemasterListRec::FixupStrings(void* record, const char* strings) {
    BattlemasterListRec* rec = static_cast<BattlemasterListRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
