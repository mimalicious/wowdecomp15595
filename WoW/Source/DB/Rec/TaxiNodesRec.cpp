#include "WoW/Source/DB/Rec/TaxiNodesRec.h"

// STATIC: %0x00B449E4
const DBCMeta TaxiNodesRec::s_meta = { "DBFilesClient\\TaxiNodes.dbc", 5, 32, TaxiNodesRec::FixupStrings, 0 };

// OFFSET: %0x0097C410
void TaxiNodesRec::FixupStrings(void* record, const char* strings) {
    TaxiNodesRec* rec = static_cast<TaxiNodesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void TaxiNodesRec::ConvertRow(TaxiNodesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_continentID, &row[0x0004], sizeof(rec->m_continentID));
    memcpy(rec->m_pos, &row[0x0008], sizeof(rec->m_pos));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    memcpy(rec->m_mountCreatureID, &row[0x0018], sizeof(rec->m_mountCreatureID));
}
#endif
