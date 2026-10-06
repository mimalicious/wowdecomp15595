#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"

// STATIC: %0x00B45374
const DBCMeta SpellCastingRequirementsRec::s_meta = { "DBFilesClient\\SpellCastingRequirements.dbc", 7, 28, SpellCastingRequirementsRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void SpellCastingRequirementsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellCastingRequirementsRec::ConvertRow(SpellCastingRequirementsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
}
#endif
