#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"

// STATIC: 0x00C2386C/0x140A18BE0
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C23880/0x140A18BF8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C23894/0x140A18C10
static const int32_t s_fieldTypes[5] = { 0, 2, 0, 0, 0 };

// STATIC: 0x00C238A8/0x140A18C28
const DBCMeta LfgDungeonGroupRec::s_meta = { "DBFilesClient\\LfgDungeonGroup.dbc", 5, 20, LfgDungeonGroupRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 67396f38 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void LfgDungeonGroupRec::FixupStrings(void* record, const char* strings) {
    LfgDungeonGroupRec* rec = static_cast<LfgDungeonGroupRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
