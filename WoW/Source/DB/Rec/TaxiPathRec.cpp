#include "WoW/Source/DB/Rec/TaxiPathRec.h"

// STATIC: %0x00B44980
const DBCMeta TaxiPathRec::s_meta = { "DBFilesClient\\TaxiPath.dbc", 4, 16, TaxiPathRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void TaxiPathRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TaxiPathRec::ConvertRow(TaxiPathRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_fromTaxiNode, &row[0x0004], sizeof(rec->m_fromTaxiNode));
    memcpy(&rec->m_toTaxiNode, &row[0x0008], sizeof(rec->m_toTaxiNode));
    memcpy(&rec->m_cost, &row[0x000C], sizeof(rec->m_cost));
}
#endif
