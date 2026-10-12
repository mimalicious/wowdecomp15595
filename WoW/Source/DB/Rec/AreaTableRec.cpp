#include "WoW/Source/DB/Rec/AreaTableRec.h"

// STATIC: 0x00C28498/0x140A1EC80
static const int32_t s_fieldOffsets[23] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 68, 72, 76, 80, 84, 88, 92, 96, 100 };

// STATIC: 0x00C284F8/0x140A1ECE0
static const int32_t s_fieldSizes[23] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C28558/0x140A1ED40
static const int32_t s_fieldTypes[23] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C285B4/0x140A1EDA0
const DBCMeta AreaTableRec::s_meta = { "DBFilesClient\\AreaTable.dbc", 23, 104, AreaTableRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EC40/0x1407D85D0
void AreaTableRec::FixupStrings(void* record, const char* strings) {
    AreaTableRec* rec = static_cast<AreaTableRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}
