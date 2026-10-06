#include "WoW/Source/DB/Rec/SkillLineCategoryRec.h"

// STATIC: %0x00B45568
const DBCMeta SkillLineCategoryRec::s_meta = { "DBFilesClient\\SkillLineCategory.dbc", 3, 12, SkillLineCategoryRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SkillLineCategoryRec::FixupStrings(void* record, const char* strings) {
    SkillLineCategoryRec* rec = static_cast<SkillLineCategoryRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SkillLineCategoryRec::ConvertRow(SkillLineCategoryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_sortIndex, &row[0x0008], sizeof(rec->m_sortIndex));
}
#endif
