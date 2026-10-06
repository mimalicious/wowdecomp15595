#include "WoW/Source/DB/Rec/SkillTiersRec.h"

// STATIC: %0x00B454C4
const DBCMeta SkillTiersRec::s_meta = { "DBFilesClient\\SkillTiers.dbc", 3, 132, SkillTiersRec::FixupStrings, 0 };

// PROVEN: 95f2390d IDENTICAL/NONE
// OFFSET: 0x00401000
void SkillTiersRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SkillTiersRec::ConvertRow(SkillTiersRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_cost, &row[0x0004], sizeof(rec->m_cost));
    memcpy(rec->m_value, &row[0x0044], sizeof(rec->m_value));
}
#endif
