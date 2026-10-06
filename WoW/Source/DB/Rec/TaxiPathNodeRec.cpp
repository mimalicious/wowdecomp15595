#include "WoW/Source/DB/Rec/TaxiPathNodeRec.h"

// STATIC: %0x00B449B4
const DBCMeta TaxiPathNodeRec::s_meta = { "DBFilesClient\\TaxiPathNode.dbc", 9, 44, TaxiPathNodeRec::FixupStrings, 0 };

// PROVEN: 5bacbe39 IDENTICAL
// OFFSET: 0x00401000
void TaxiPathNodeRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TaxiPathNodeRec::ConvertRow(TaxiPathNodeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_pathID, &row[0x0004], sizeof(rec->m_pathID));
    memcpy(&rec->m_nodeIndex, &row[0x0008], sizeof(rec->m_nodeIndex));
    memcpy(&rec->m_continentID, &row[0x000C], sizeof(rec->m_continentID));
    memcpy(rec->m_loc, &row[0x0010], sizeof(rec->m_loc));
    memcpy(&rec->m_flags, &row[0x001C], sizeof(rec->m_flags));
    memcpy(&rec->m_delay, &row[0x0020], sizeof(rec->m_delay));
    memcpy(&rec->m_arrivalEventID, &row[0x0024], sizeof(rec->m_arrivalEventID));
    memcpy(&rec->m_departureEventID, &row[0x0028], sizeof(rec->m_departureEventID));
}
#endif
