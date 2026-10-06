#include "WoW/Source/DB/Rec/CharBaseInfoRec.h"

// STATIC: %0x00B43FB4
const DBCMeta CharBaseInfoRec::s_meta = { "DBFilesClient\\CharBaseInfo.dbc", 2, 8, CharBaseInfoRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void CharBaseInfoRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CharBaseInfoRec::ConvertRow(CharBaseInfoRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_raceID, &row[0x0004], sizeof(rec->m_raceID));
    memcpy(&rec->m_classID, &row[0x0005], sizeof(rec->m_classID));
}
#endif
