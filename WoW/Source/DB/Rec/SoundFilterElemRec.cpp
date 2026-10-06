#include "WoW/Source/DB/Rec/SoundFilterElemRec.h"

// STATIC: %0x00B435FC
const DBCMeta SoundFilterElemRec::s_meta = { "DBFilesClient\\SoundFilterElem.dbc", 5, 52, SoundFilterElemRec::FixupStrings, 0 };

// PROVEN: b4917fee IDENTICAL
// OFFSET: 0x00401000
void SoundFilterElemRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SoundFilterElemRec::ConvertRow(SoundFilterElemRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_soundFilterID, &row[0x0004], sizeof(rec->m_soundFilterID));
    memcpy(&rec->m_orderIndex, &row[0x0008], sizeof(rec->m_orderIndex));
    memcpy(&rec->m_filterType, &row[0x000C], sizeof(rec->m_filterType));
    memcpy(rec->m_params, &row[0x0010], sizeof(rec->m_params));
}
#endif
