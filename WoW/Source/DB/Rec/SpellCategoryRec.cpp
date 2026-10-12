#include "WoW/Source/DB/Rec/SpellCategoryRec.h"

// STATIC: 0x00C21D10/0x140A169C8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C21D20/0x140A169D8
static const int32_t s_fieldSizes[4] = { 4, 4, 1, 4 };

// STATIC: 0x00C21D30/0x140A169E8
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 2 };

// STATIC: 0x00C21D40/0x140A169F8
const DBCMeta SpellCategoryRec::s_meta = { "DBFilesClient\\SpellCategory.dbc", 4, 16, SpellCategoryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECE0/0x1407D8380
void SpellCategoryRec::FixupStrings(void* record, const char* strings) {
    SpellCategoryRec* rec = static_cast<SpellCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
