#include "WoW/Source/DB/Rec/SkillLineRec.h"

// STATIC: %0x00B45530
const DBCMeta SkillLineRec::s_meta = { "DBFilesClient\\SkillLine.dbc", 8, 32, SkillLineRec::FixupStrings, 0 };

// OFFSET: %0x0097C290
void SkillLineRec::FixupStrings(void* record, const char* strings) {
    SkillLineRec* rec = static_cast<SkillLineRec*>(record);
    rec->m_displayName = &strings[reinterpret_cast<uintptr_t>(rec->m_displayName)];
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
    rec->m_alternateVerb = &strings[reinterpret_cast<uintptr_t>(rec->m_alternateVerb)];
}

#if defined(WOWDECOMP_ARCH_64)
void SkillLineRec::ConvertRow(SkillLineRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_categoryID, &row[0x0004], sizeof(rec->m_categoryID));
    memcpy(&rec->m_skillCostsID, &row[0x0008], sizeof(rec->m_skillCostsID));
    rec->m_displayName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    memcpy(&rec->m_spellIconID, &row[0x0014], sizeof(rec->m_spellIconID));
    rec->m_alternateVerb = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(&rec->m_canLink, &row[0x001C], sizeof(rec->m_canLink));
}
#endif
