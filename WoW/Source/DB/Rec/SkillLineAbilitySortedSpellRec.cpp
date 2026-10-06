#include "WoW/Source/DB/Rec/SkillLineAbilitySortedSpellRec.h"

// STATIC: %0x00B455AC
const DBCMeta SkillLineAbilitySortedSpellRec::s_meta = { "DBFilesClient\\SkillLineAbilitySortedSpell.dbc", 2, 8, SkillLineAbilitySortedSpellRec::FixupStrings, 0 };

// PROVEN: 32de84f7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SkillLineAbilitySortedSpellRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SkillLineAbilitySortedSpellRec::ConvertRow(SkillLineAbilitySortedSpellRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
