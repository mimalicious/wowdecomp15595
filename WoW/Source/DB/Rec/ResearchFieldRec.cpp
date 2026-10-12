#include "WoW/Source/DB/Rec/ResearchFieldRec.h"

// STATIC: 0x00C22B14/0x140A17B00
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C22B20/0x140A17B10
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C22B2C/0x140A17B20
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C22B38/0x140A17B30
const DBCMeta ResearchFieldRec::s_meta = { "DBFilesClient\\ResearchField.dbc", 3, 12, ResearchFieldRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 97d47da1 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void ResearchFieldRec::FixupStrings(void* record, const char* strings) {
    ResearchFieldRec* rec = static_cast<ResearchFieldRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
