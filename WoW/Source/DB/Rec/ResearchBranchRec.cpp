#include "WoW/Source/DB/Rec/ResearchBranchRec.h"

// STATIC: %0x00B45810
const DBCMeta ResearchBranchRec::s_meta = { "DBFilesClient\\ResearchBranch.dbc", 6, 24, ResearchBranchRec::FixupStrings, 0 };

// OFFSET: %0x0097C2D0
void ResearchBranchRec::FixupStrings(void* record, const char* strings) {
    ResearchBranchRec* rec = static_cast<ResearchBranchRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
    rec->unk_0010 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0010)];
}

#if defined(WOWDECOMP_ARCH_64)
void ResearchBranchRec::ConvertRow(ResearchBranchRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    rec->unk_0010 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
}
#endif
