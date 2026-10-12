#include "WoW/Source/DB/Rec/BattlemasterListRec.h"

// STATIC: 0x00C26778/0x140A1C738
static const int32_t s_fieldOffsets[13] = { 0, 4, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76 };

// STATIC: 0x00C267AC/0x140A1C770
static const int32_t s_fieldSizes[13] = { 4, 32, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C267E0/0x140A1C7A8
static const int32_t s_fieldTypes[13] = { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C26814/0x140A1C7E0
const DBCMeta BattlemasterListRec::s_meta = { "DBFilesClient\\BattlemasterList.dbc", 13, 80, BattlemasterListRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EC40/0x1407D85D0
void BattlemasterListRec::FixupStrings(void* record, const char* strings) {
    BattlemasterListRec* rec = static_cast<BattlemasterListRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
