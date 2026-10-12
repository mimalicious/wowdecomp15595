#include "WoW/Source/DB/Rec/GameTablesRec.h"

// STATIC: 0x00C253A8/0x140A1AF00
static const int32_t s_fieldOffsets[3] = { 4, 8, 12 };

// STATIC: 0x00C253B4/0x140A1AF10
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C253C0/0x140A1AF20
static const int32_t s_fieldTypes[3] = { 2, 0, 0 };

// STATIC: 0x00C253CC/0x140A1AF30
const DBCMeta GameTablesRec::s_meta = { "DBFilesClient\\GameTables.dbc", 3, 16, GameTablesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 47ff8843 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void GameTablesRec::FixupStrings(void* record, const char* strings) {
    GameTablesRec* rec = static_cast<GameTablesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
