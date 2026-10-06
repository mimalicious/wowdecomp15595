#include "WoW/Source/DB/Rec/TransportRotationRec.h"

// STATIC: %0x00B4483C
const DBCMeta TransportRotationRec::s_meta = { "DBFilesClient\\TransportRotation.dbc", 4, 28, TransportRotationRec::FixupStrings, 0 };

// PROVEN: 0b1f474d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TransportRotationRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TransportRotationRec::ConvertRow(TransportRotationRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_gameObjectsID, &row[0x0004], sizeof(rec->m_gameObjectsID));
    memcpy(&rec->m_timeIndex, &row[0x0008], sizeof(rec->m_timeIndex));
    memcpy(rec->m_rot, &row[0x000C], sizeof(rec->m_rot));
}
#endif
