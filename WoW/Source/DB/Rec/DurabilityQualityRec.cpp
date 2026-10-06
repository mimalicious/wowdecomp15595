#include "WoW/Source/DB/Rec/DurabilityQualityRec.h"

// STATIC: %0x00B46C40
const DBCMeta DurabilityQualityRec::s_meta = { "DBFilesClient\\DurabilityQuality.dbc", 2, 8, DurabilityQualityRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void DurabilityQualityRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void DurabilityQualityRec::ConvertRow(DurabilityQualityRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
