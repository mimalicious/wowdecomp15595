#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"

// STATIC: 0x00C23FC8/0x140A19500
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 28 };

// STATIC: 0x00C23FD8/0x140A19510
static const int32_t s_fieldSizes[4] = { 4, 4, 20, 4 };

// STATIC: 0x00C23FE8/0x140A19520
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 2 };

// STATIC: 0x00C23FF8/0x140A19530
const DBCMeta ItemRandomPropertiesRec::s_meta = { "DBFilesClient\\ItemRandomProperties.dbc", 4, 32, ItemRandomPropertiesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 9d677c96 IDENTICAL/NONE
// OFFSET: 0x00A2EB50/0x1407D82D0
void ItemRandomPropertiesRec::FixupStrings(void* record, const char* strings) {
    ItemRandomPropertiesRec* rec = static_cast<ItemRandomPropertiesRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_nameLang, strings);
}
