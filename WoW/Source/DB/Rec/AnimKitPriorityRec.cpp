#include "WoW/Source/DB/Rec/AnimKitPriorityRec.h"

// STATIC: %0x00B44168
const DBCMeta AnimKitPriorityRec::s_meta = { "DBFilesClient\\AnimKitPriority.dbc", 2, 8, AnimKitPriorityRec::FixupStrings, 0 };

// PROVEN: 80b60700 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimKitPriorityRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AnimKitPriorityRec::ConvertRow(AnimKitPriorityRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
