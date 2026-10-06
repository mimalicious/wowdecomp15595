#include "WoW/Source/DB/Rec/AnimKitConfigBoneSetRec.h"

// STATIC: %0x00B441A4
const DBCMeta AnimKitConfigBoneSetRec::s_meta = { "DBFilesClient\\AnimKitConfigBoneSet.dbc", 4, 16, AnimKitConfigBoneSetRec::FixupStrings, 0 };

// PROVEN: f4fa01e6 IDENTICAL/NONE
// OFFSET: 0x00401000
void AnimKitConfigBoneSetRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AnimKitConfigBoneSetRec::ConvertRow(AnimKitConfigBoneSetRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
