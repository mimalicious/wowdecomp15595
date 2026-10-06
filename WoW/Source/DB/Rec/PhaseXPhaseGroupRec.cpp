#include "WoW/Source/DB/Rec/PhaseXPhaseGroupRec.h"

// STATIC: %0x00B459E4
const DBCMeta PhaseXPhaseGroupRec::s_meta = { "DBFilesClient\\PhaseXPhaseGroup.dbc", 3, 12, PhaseXPhaseGroupRec::FixupStrings, 0 };

// PROVEN: aa38a3d7 IDENTICAL/NONE
// OFFSET: 0x00401000
void PhaseXPhaseGroupRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void PhaseXPhaseGroupRec::ConvertRow(PhaseXPhaseGroupRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
}
#endif
