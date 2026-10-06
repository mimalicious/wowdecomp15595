#include "WoW/Source/DB/Rec/SkillLineCategoryRec.h"

// STATIC: 0x00C224A0/0x140A17340
const DBCMeta SkillLineCategoryRec::s_meta = { "DBFilesClient\\SkillLineCategory.dbc", 3, 12, SkillLineCategoryRec::FixupStrings, 0 };

// PROVEN: db9b2de8 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SkillLineCategoryRec::FixupStrings(void* record, const char* strings) {
    SkillLineCategoryRec* rec = static_cast<SkillLineCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
