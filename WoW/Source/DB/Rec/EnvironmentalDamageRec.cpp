#include "WoW/Source/DB/Rec/EnvironmentalDamageRec.h"

// STATIC: %0x00B46B34
const DBCMeta EnvironmentalDamageRec::s_meta = { "DBFilesClient\\EnvironmentalDamage.dbc", 3, 12, EnvironmentalDamageRec::FixupStrings, 0 };

// PROVEN: 20f306ec IDENTICAL/NONE
// OFFSET: 0x00401000
void EnvironmentalDamageRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void EnvironmentalDamageRec::ConvertRow(EnvironmentalDamageRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_enumID, &row[0x0004], sizeof(rec->m_enumID));
    memcpy(&rec->m_visualkitID, &row[0x0008], sizeof(rec->m_visualkitID));
}
#endif
