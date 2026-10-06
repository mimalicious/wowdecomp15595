#include "WoW/Source/DB/Rec/AnimKitSegmentRec.h"

// STATIC: %0x00B44130
const DBCMeta AnimKitSegmentRec::s_meta = { "DBFilesClient\\AnimKitSegment.dbc", 16, 64, AnimKitSegmentRec::FixupStrings, 0 };

// PROVEN: ee1aba6e IDENTICAL
// OFFSET: 0x00401000
void AnimKitSegmentRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AnimKitSegmentRec::ConvertRow(AnimKitSegmentRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
    memcpy(&rec->unk_003C, &row[0x003C], sizeof(rec->unk_003C));
}
#endif
