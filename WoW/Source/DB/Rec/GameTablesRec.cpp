#include "WoW/Source/DB/Rec/GameTablesRec.h"

// STATIC: 0x00C253CC/0x140A1AF30
const DBCMeta GameTablesRec::s_meta = { "DBFilesClient\\GameTables.dbc", 3, 16, GameTablesRec::FixupStrings, 0 };

// PROVEN: 47ff8843 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GameTablesRec::FixupStrings(void* record, const char* strings) {
    GameTablesRec* rec = static_cast<GameTablesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
