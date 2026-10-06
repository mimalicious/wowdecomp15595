#include "WoW/Source/DB/Rec/ZoneLightRec.h"

// STATIC: %0x00B443C4
const DBCMeta ZoneLightRec::s_meta = { "DBFilesClient\\ZoneLight.dbc", 4, 16, ZoneLightRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ZoneLightRec::FixupStrings(void* record, const char* strings) {
    ZoneLightRec* rec = static_cast<ZoneLightRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
}

#if defined(WOWDECOMP_ARCH_64)
void ZoneLightRec::ConvertRow(ZoneLightRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
