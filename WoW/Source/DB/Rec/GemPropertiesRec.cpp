#include "WoW/Source/DB/Rec/GemPropertiesRec.h"

// STATIC: %0x00B46978
const DBCMeta GemPropertiesRec::s_meta = { "DBFilesClient\\GemProperties.dbc", 6, 24, GemPropertiesRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void GemPropertiesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GemPropertiesRec::ConvertRow(GemPropertiesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_enchantID, &row[0x0004], sizeof(rec->m_enchantID));
    memcpy(&rec->m_maxcountInv, &row[0x0008], sizeof(rec->m_maxcountInv));
    memcpy(&rec->m_maxcountItem, &row[0x000C], sizeof(rec->m_maxcountItem));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
}
#endif
