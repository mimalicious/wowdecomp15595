#include "WoW/Source/DB/Rec/ScreenLocationRec.h"

// STATIC: %0x00B45654
const DBCMeta ScreenLocationRec::s_meta = { "DBFilesClient\\ScreenLocation.dbc", 2, 8, ScreenLocationRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ScreenLocationRec::FixupStrings(void* record, const char* strings) {
    ScreenLocationRec* rec = static_cast<ScreenLocationRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
}

#if defined(WOWDECOMP_ARCH_64)
void ScreenLocationRec::ConvertRow(ScreenLocationRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
