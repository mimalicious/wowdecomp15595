#include "WoW/Source/DB/Rec/ResearchFieldRec.h"

// STATIC: %0x00B457D8
const DBCMeta ResearchFieldRec::s_meta = { "DBFilesClient\\ResearchField.dbc", 3, 12, ResearchFieldRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ResearchFieldRec::FixupStrings(void* record, const char* strings) {
    ResearchFieldRec* rec = static_cast<ResearchFieldRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
}

#if defined(WOWDECOMP_ARCH_64)
void ResearchFieldRec::ConvertRow(ResearchFieldRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
}
#endif
