#include "WoW/Source/DB/Rec/CreatureMovementInfoRec.h"

// STATIC: %0x00B46F10
const DBCMeta CreatureMovementInfoRec::s_meta = { "DBFilesClient\\CreatureMovementInfo.dbc", 2, 8, CreatureMovementInfoRec::FixupStrings, 0 };

// PROVEN: 4ca884c1 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CreatureMovementInfoRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CreatureMovementInfoRec::ConvertRow(CreatureMovementInfoRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_smoothFacingChaseRate, &row[0x0004], sizeof(rec->m_smoothFacingChaseRate));
}
#endif
