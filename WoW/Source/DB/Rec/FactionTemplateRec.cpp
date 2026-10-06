#include "WoW/Source/DB/Rec/FactionTemplateRec.h"

// STATIC: %0x00B43CE4
const DBCMeta FactionTemplateRec::s_meta = { "DBFilesClient\\FactionTemplate.dbc", 8, 56, FactionTemplateRec::FixupStrings, 0 };

// PROVEN: c85fd199 IDENTICAL
// OFFSET: 0x00401000
void FactionTemplateRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void FactionTemplateRec::ConvertRow(FactionTemplateRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_faction, &row[0x0004], sizeof(rec->m_faction));
    memcpy(&rec->m_flags, &row[0x0008], sizeof(rec->m_flags));
    memcpy(&rec->m_factionGroup, &row[0x000C], sizeof(rec->m_factionGroup));
    memcpy(&rec->m_friendGroup, &row[0x0010], sizeof(rec->m_friendGroup));
    memcpy(&rec->m_enemyGroup, &row[0x0014], sizeof(rec->m_enemyGroup));
    memcpy(rec->m_enemies, &row[0x0018], sizeof(rec->m_enemies));
    memcpy(rec->m_friend, &row[0x0028], sizeof(rec->m_friend));
}
#endif
