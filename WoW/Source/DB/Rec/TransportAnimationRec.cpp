#include "WoW/Source/DB/Rec/TransportAnimationRec.h"

// STATIC: %0x00B448B0
const DBCMeta TransportAnimationRec::s_meta = { "DBFilesClient\\TransportAnimation.dbc", 5, 28, TransportAnimationRec::FixupStrings, 0 };

// PROVEN: 480c9e45 IDENTICAL
// OFFSET: 0x00401000
void TransportAnimationRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TransportAnimationRec::ConvertRow(TransportAnimationRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_transportID, &row[0x0004], sizeof(rec->m_transportID));
    memcpy(&rec->m_timeIndex, &row[0x0008], sizeof(rec->m_timeIndex));
    memcpy(rec->m_pos, &row[0x000C], sizeof(rec->m_pos));
    memcpy(&rec->m_sequenceID, &row[0x0018], sizeof(rec->m_sequenceID));
}
#endif
