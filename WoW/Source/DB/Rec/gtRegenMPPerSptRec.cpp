#include "WoW/Source/DB/Rec/gtRegenMPPerSptRec.h"

// STATIC: %0x00B4652C
const DBCMeta gtRegenMPPerSptRec::s_meta = { "DBFilesClient\\gtRegenMPPerSpt.dbc", 1, 8, gtRegenMPPerSptRec::FixupStrings, 0 };

// PROVEN: c4ebab3d IDENTICAL
// OFFSET: 0x00401000
void gtRegenMPPerSptRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtRegenMPPerSptRec::ConvertRow(gtRegenMPPerSptRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
