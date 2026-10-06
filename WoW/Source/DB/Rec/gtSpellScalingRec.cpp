#include "WoW/Source/DB/Rec/gtSpellScalingRec.h"

// STATIC: %0x00B464F4
const DBCMeta gtSpellScalingRec::s_meta = { "DBFilesClient\\gtSpellScaling.dbc", 1, 8, gtSpellScalingRec::FixupStrings, 0 };

// PROVEN: 286b55bc IDENTICAL/NONE
// OFFSET: 0x00401000
void gtSpellScalingRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtSpellScalingRec::ConvertRow(gtSpellScalingRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
