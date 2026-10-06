#include "WoW/Source/DB/Rec/AnimReplacementSetRec.h"

// STATIC: %0x00B440C0
const DBCMeta AnimReplacementSetRec::s_meta = { "DBFilesClient\\AnimReplacementSet.dbc", 2, 8, AnimReplacementSetRec::FixupStrings, 0 };

// PROVEN: 28906208 IDENTICAL/NONE
// OFFSET: 0x00401000
void AnimReplacementSetRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AnimReplacementSetRec::ConvertRow(AnimReplacementSetRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
