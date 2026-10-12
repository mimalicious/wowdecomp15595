#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"

// STATIC: 0x00C23F44/0x140A19458
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 32 };

// STATIC: 0x00C23F58/0x140A19470
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 20, 20 };

// STATIC: 0x00C23F6C/0x140A19488
static const int32_t s_fieldTypes[5] = { 0, 2, 2, 0, 0 };

// STATIC: 0x00C23F80/0x140A194A0
const DBCMeta ItemRandomSuffixRec::s_meta = { "DBFilesClient\\ItemRandomSuffix.dbc", 5, 52, ItemRandomSuffixRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E9B0/0x1407D8110
void ItemRandomSuffixRec::FixupStrings(void* record, const char* strings) {
    ItemRandomSuffixRec* rec = static_cast<ItemRandomSuffixRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_internalName, strings);
}
