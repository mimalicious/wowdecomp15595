#include "WoW/Source/DB/Rec/ResearchProjectRec.h"

// STATIC: %0x00B457A4
const DBCMeta ResearchProjectRec::s_meta = { "DBFilesClient\\ResearchProject.dbc", 9, 36, ResearchProjectRec::FixupStrings, 0 };

// OFFSET: %0x0097C2B0
void ResearchProjectRec::FixupStrings(void* record, const char* strings) {
    ResearchProjectRec* rec = static_cast<ResearchProjectRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
    rec->unk_0008 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0008)];
    rec->unk_001C = &strings[reinterpret_cast<uintptr_t>(rec->unk_001C)];
}

#if defined(WOWDECOMP_ARCH_64)
void ResearchProjectRec::ConvertRow(ResearchProjectRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->unk_0008 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    rec->unk_001C = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
}
#endif
