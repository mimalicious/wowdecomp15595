#include "WoW/Source/DB/Rec/RandPropPointsRec.h"

// STATIC: %0x00B45738
const DBCMeta RandPropPointsRec::s_meta = { "DBFilesClient\\RandPropPoints.dbc", 4, 64, RandPropPointsRec::FixupStrings, 0 };

// PROVEN: 847b9dd3 IDENTICAL
// OFFSET: 0x00401000
void RandPropPointsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void RandPropPointsRec::ConvertRow(RandPropPointsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_epic, &row[0x0004], sizeof(rec->m_epic));
    memcpy(rec->m_superior, &row[0x0018], sizeof(rec->m_superior));
    memcpy(rec->m_good, &row[0x002C], sizeof(rec->m_good));
}
#endif
