#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"

// STATIC: 0x00C26DB8/0x140A1CEE0
static const int32_t s_fieldOffsets[19] = { 0, 4, 8, 12, 16, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 84 };

// STATIC: 0x00C26E08/0x140A1CF30
static const int32_t s_fieldSizes[19] = { 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 12 };

// STATIC: 0x00C26E58/0x140A1CF80
static const int32_t s_fieldTypes[19] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C26EA4/0x140A1CFD0
const DBCMeta Achievement_CriteriaRec::s_meta = { "DBFilesClient\\Achievement_Criteria.dbc", 19, 96, Achievement_CriteriaRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 88a189fa IDENTICAL/DIFFERENT
// OFFSET: 0x00A2ECB0/0x1407D8650
void Achievement_CriteriaRec::FixupStrings(void* record, const char* strings) {
    Achievement_CriteriaRec* rec = static_cast<Achievement_CriteriaRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
