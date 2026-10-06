#include "WoW/Source/DB/Rec/AnimKitBoneSetRec.h"

// STATIC: %0x00B44240
const DBCMeta AnimKitBoneSetRec::s_meta = { "DBFilesClient\\AnimKitBoneSet.dbc", 6, 24, AnimKitBoneSetRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void AnimKitBoneSetRec::FixupStrings(void* record, const char* strings) {
    AnimKitBoneSetRec* rec = static_cast<AnimKitBoneSetRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
}

#if defined(WOWDECOMP_ARCH_64)
void AnimKitBoneSetRec::ConvertRow(AnimKitBoneSetRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
}
#endif
