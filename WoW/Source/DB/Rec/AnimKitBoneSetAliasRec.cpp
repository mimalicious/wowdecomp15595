#include "WoW/Source/DB/Rec/AnimKitBoneSetAliasRec.h"

// STATIC: %0x00B4427C
const DBCMeta AnimKitBoneSetAliasRec::s_meta = { "DBFilesClient\\AnimKitBoneSetAlias.dbc", 3, 12, AnimKitBoneSetAliasRec::FixupStrings, 0 };

// PROVEN: eaa447dc IDENTICAL/NONE
// OFFSET: 0x00401000
void AnimKitBoneSetAliasRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AnimKitBoneSetAliasRec::ConvertRow(AnimKitBoneSetAliasRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
}
#endif
