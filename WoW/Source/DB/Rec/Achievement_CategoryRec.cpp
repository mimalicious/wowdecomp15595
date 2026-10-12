#include "WoW/Source/DB/Rec/Achievement_CategoryRec.h"

// STATIC: 0x00C26D40/0x140A1CE50
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C26D50/0x140A1CE60
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C26D60/0x140A1CE70
static const int32_t s_fieldTypes[4] = { 0, 0, 2, 0 };

// STATIC: 0x00C26D70/0x140A1CE80
const DBCMeta Achievement_CategoryRec::s_meta = { "DBFilesClient\\Achievement_Category.dbc", 4, 16, Achievement_CategoryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECA0/0x1407D8230
void Achievement_CategoryRec::FixupStrings(void* record, const char* strings) {
    Achievement_CategoryRec* rec = static_cast<Achievement_CategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
