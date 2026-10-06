#include "WoW/Source/DB/Rec/gtOCTRegenMPRec.h"

// STATIC: %0x00B46560
const DBCMeta gtOCTRegenMPRec::s_meta = { "DBFilesClient\\gtOCTRegenMP.dbc", 1, 8, gtOCTRegenMPRec::FixupStrings, 0 };

// PROVEN: ec1245e4 IDENTICAL/NONE
// OFFSET: 0x00401000
void gtOCTRegenMPRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtOCTRegenMPRec::ConvertRow(gtOCTRegenMPRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
