#include "WoW/Source/DB/Rec/SpellLevelsRec.h"

// STATIC: %0x00B44ED0
const DBCMeta SpellLevelsRec::s_meta = { "DBFilesClient\\SpellLevels.dbc", 4, 16, SpellLevelsRec::FixupStrings, 0 };

// PROVEN: 6426bab5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellLevelsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellLevelsRec::ConvertRow(SpellLevelsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
