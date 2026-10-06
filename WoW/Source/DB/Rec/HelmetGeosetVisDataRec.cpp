#include "WoW/Source/DB/Rec/HelmetGeosetVisDataRec.h"

// STATIC: %0x00B43B2C
const DBCMeta HelmetGeosetVisDataRec::s_meta = { "DBFilesClient\\HelmetGeosetVisData.dbc", 2, 32, HelmetGeosetVisDataRec::FixupStrings, 0 };

// PROVEN: 938efb8d IDENTICAL
// OFFSET: 0x00401000
void HelmetGeosetVisDataRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void HelmetGeosetVisDataRec::ConvertRow(HelmetGeosetVisDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_hideGeoset, &row[0x0004], sizeof(rec->m_hideGeoset));
}
#endif
