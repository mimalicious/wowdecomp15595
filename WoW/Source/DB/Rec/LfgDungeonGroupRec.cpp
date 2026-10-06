#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"

// STATIC: 0x00C238A8/0x140A18C28
const DBCMeta LfgDungeonGroupRec::s_meta = { "DBFilesClient\\LfgDungeonGroup.dbc", 5, 20, LfgDungeonGroupRec::FixupStrings, 0 };

// PROVEN: 67396f38 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void LfgDungeonGroupRec::FixupStrings(void* record, const char* strings) {
    LfgDungeonGroupRec* rec = static_cast<LfgDungeonGroupRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
