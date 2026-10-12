#include "WoW/Source/DB/Rec/SkillLineCategoryRec.h"

// STATIC: 0x00C2247C/0x140A17310
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C22488/0x140A17320
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C22494/0x140A17330
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C224A0/0x140A17340
const DBCMeta SkillLineCategoryRec::s_meta = { "DBFilesClient\\SkillLineCategory.dbc", 3, 12, SkillLineCategoryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: db9b2de8 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void SkillLineCategoryRec::FixupStrings(void* record, const char* strings) {
    SkillLineCategoryRec* rec = static_cast<SkillLineCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
