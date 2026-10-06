#include "WoW/Source/DB/Rec/LightIntBandRec.h"

// STATIC: %0x00B439BC
const DBCMeta LightIntBandRec::s_meta = { "DBFilesClient\\LightIntBand.dbc", 4, 136, LightIntBandRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void LightIntBandRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LightIntBandRec::ConvertRow(LightIntBandRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_num, &row[0x0004], sizeof(rec->m_num));
    memcpy(rec->m_time, &row[0x0008], sizeof(rec->m_time));
    memcpy(rec->m_data, &row[0x0048], sizeof(rec->m_data));
}
#endif
