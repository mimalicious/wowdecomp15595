#include "WoW/Source/DB/Rec/CharTitlesRec.h"

// STATIC: 0x00C264FC/0x140A1C430
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C26514/0x140A1C448
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C2652C/0x140A1C460
static const int32_t s_fieldTypes[6] = { 0, 0, 2, 2, 0, 0 };

// STATIC: 0x00C26544/0x140A1C478
const DBCMeta CharTitlesRec::s_meta = { "DBFilesClient\\CharTitles.dbc", 6, 24, CharTitlesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EC50/0x1407D8590
void CharTitlesRec::FixupStrings(void* record, const char* strings) {
    CharTitlesRec* rec = static_cast<CharTitlesRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_name1, strings);
}
