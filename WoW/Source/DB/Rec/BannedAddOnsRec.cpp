#include "WoW/Source/DB/Rec/BannedAddOnsRec.h"

// STATIC: %0x00B44054
const DBCMeta BannedAddOnsRec::s_meta = { "DBFilesClient\\BannedAddOns.dbc", 5, 44, BannedAddOnsRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void BannedAddOnsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void BannedAddOnsRec::ConvertRow(BannedAddOnsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_nameMd5, &row[0x0004], sizeof(rec->m_nameMd5));
    memcpy(rec->m_versionMd5, &row[0x0014], sizeof(rec->m_versionMd5));
    memcpy(&rec->m_lastModified, &row[0x0024], sizeof(rec->m_lastModified));
    memcpy(&rec->m_flags, &row[0x0028], sizeof(rec->m_flags));
}
#endif
