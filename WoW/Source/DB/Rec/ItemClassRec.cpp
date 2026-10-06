#include "WoW/Source/DB/Rec/ItemClassRec.h"

// STATIC: 0x00C1E288/0x140A12448
static const int32_t s_fieldOffsets[5] = { 4, 8, 12, 16, 20 };

// STATIC: 0x00C1E29C/0x140A12460
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C1E2B0/0x140A12478
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 3, 2 };

// STATIC: 0x00C1E2C4/0x140A12490
const DBCMeta ItemClassRec::s_meta = { "DBFilesClient\\ItemClass.dbc", 5, 24, ItemClassRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 20783a7b IDENTICAL/NONE
// OFFSET: 0x00A2EBF0/0x1407D84B0
void ItemClassRec::FixupStrings(void* record, const char* strings) {
    ItemClassRec* rec = static_cast<ItemClassRec*>(record);
    DBCFixupString(rec->m_className, strings);
}
