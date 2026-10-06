#include "WoW/Source/DB/Rec/ResearchSiteRec.h"

// STATIC: %0x00B4576C
const DBCMeta ResearchSiteRec::s_meta = { "DBFilesClient\\ResearchSite.dbc", 5, 20, ResearchSiteRec::FixupStrings, 0 };

// OFFSET: %0x0097C4E0
void ResearchSiteRec::FixupStrings(void* record, const char* strings) {
    ResearchSiteRec* rec = static_cast<ResearchSiteRec*>(record);
    rec->unk_000C = &strings[reinterpret_cast<uintptr_t>(rec->unk_000C)];
}

#if defined(WOWDECOMP_ARCH_64)
void ResearchSiteRec::ConvertRow(ResearchSiteRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    rec->unk_000C = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
}
#endif
