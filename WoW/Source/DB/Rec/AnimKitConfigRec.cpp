#include "WoW/Source/DB/Rec/AnimKitConfigRec.h"

// STATIC: %0x00B441D8
const DBCMeta AnimKitConfigRec::s_meta = { "DBFilesClient\\AnimKitConfig.dbc", 2, 8, AnimKitConfigRec::FixupStrings, 0 };

// PROVEN: 6975b215 IDENTICAL
// OFFSET: 0x00401000
void AnimKitConfigRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AnimKitConfigRec::ConvertRow(AnimKitConfigRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
