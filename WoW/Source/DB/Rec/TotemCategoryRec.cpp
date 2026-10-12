#include "WoW/Source/DB/Rec/TotemCategoryRec.h"

// STATIC: 0x00C1FFFC/0x140A14738
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C2000C/0x140A14748
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C2001C/0x140A14758
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 0 };

// STATIC: 0x00C2002C/0x140A14768
const DBCMeta TotemCategoryRec::s_meta = { "DBFilesClient\\TotemCategory.dbc", 4, 16, TotemCategoryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void TotemCategoryRec::FixupStrings(void* record, const char* strings) {
    TotemCategoryRec* rec = static_cast<TotemCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
