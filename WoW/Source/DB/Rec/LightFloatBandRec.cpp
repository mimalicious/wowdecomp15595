#include "WoW/Source/DB/Rec/LightFloatBandRec.h"

// STATIC: %0x00B45D94
const DBCMeta LightFloatBandRec::s_meta = { "DBFilesClient\\LightFloatBand.dbc", 4, 136, LightFloatBandRec::FixupStrings, 0 };

// PROVEN: 6e037d49 IDENTICAL
// OFFSET: 0x00401000
void LightFloatBandRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LightFloatBandRec::ConvertRow(LightFloatBandRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_num, &row[0x0004], sizeof(rec->m_num));
    memcpy(rec->m_time, &row[0x0008], sizeof(rec->m_time));
    memcpy(rec->m_data, &row[0x0048], sizeof(rec->m_data));
}
#endif
