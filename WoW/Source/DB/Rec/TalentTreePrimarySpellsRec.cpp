#include "WoW/Source/DB/Rec/TalentTreePrimarySpellsRec.h"

// STATIC: %0x00B44A24
const DBCMeta TalentTreePrimarySpellsRec::s_meta = { "DBFilesClient\\TalentTreePrimarySpells.dbc", 4, 16, TalentTreePrimarySpellsRec::FixupStrings, 0 };

// PROVEN: 1f3fdbf7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TalentTreePrimarySpellsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TalentTreePrimarySpellsRec::ConvertRow(TalentTreePrimarySpellsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
