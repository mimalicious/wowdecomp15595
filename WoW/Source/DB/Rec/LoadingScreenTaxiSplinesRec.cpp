#include "WoW/Source/DB/Rec/LoadingScreenTaxiSplinesRec.h"

// STATIC: %0x00B45D28
const DBCMeta LoadingScreenTaxiSplinesRec::s_meta = { "DBFilesClient\\LoadingScreenTaxiSplines.dbc", 5, 76, LoadingScreenTaxiSplinesRec::FixupStrings, 0 };

// PROVEN: c72bd4ab IDENTICAL
// OFFSET: 0x00401000
void LoadingScreenTaxiSplinesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LoadingScreenTaxiSplinesRec::ConvertRow(LoadingScreenTaxiSplinesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_pathID, &row[0x0004], sizeof(rec->m_pathID));
    memcpy(rec->m_locx, &row[0x0008], sizeof(rec->m_locx));
    memcpy(rec->m_locy, &row[0x0028], sizeof(rec->m_locy));
    memcpy(&rec->m_legIndex, &row[0x0048], sizeof(rec->m_legIndex));
}
#endif
