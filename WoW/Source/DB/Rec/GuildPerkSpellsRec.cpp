#include "WoW/Source/DB/Rec/GuildPerkSpellsRec.h"

// STATIC: %0x00B464BC
const DBCMeta GuildPerkSpellsRec::s_meta = { "DBFilesClient\\GuildPerkSpells.dbc", 3, 12, GuildPerkSpellsRec::FixupStrings, 0 };

// PROVEN: f0eb869f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildPerkSpellsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GuildPerkSpellsRec::ConvertRow(GuildPerkSpellsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
}
#endif
