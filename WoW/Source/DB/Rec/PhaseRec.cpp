#include "WoW/Source/DB/Rec/PhaseRec.h"

// STATIC: %0x00B45A10
const DBCMeta PhaseRec::s_meta = { "DBFilesClient\\Phase.dbc", 3, 12, PhaseRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void PhaseRec::FixupStrings(void* record, const char* strings) {
    PhaseRec* rec = static_cast<PhaseRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
}

#if defined(WOWDECOMP_ARCH_64)
void PhaseRec::ConvertRow(PhaseRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
}
#endif
