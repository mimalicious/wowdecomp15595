#include "WoW/Source/DB/Rec/VocalUISoundsRec.h"

// STATIC: %0x00B44690
const DBCMeta VocalUISoundsRec::s_meta = { "DBFilesClient\\VocalUISounds.dbc", 5, 28, VocalUISoundsRec::FixupStrings, 0 };

// PROVEN: 4871e926 IDENTICAL/NONE
// OFFSET: 0x00401000
void VocalUISoundsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void VocalUISoundsRec::ConvertRow(VocalUISoundsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_vocalUienum, &row[0x0004], sizeof(rec->m_vocalUienum));
    memcpy(&rec->m_raceID, &row[0x0008], sizeof(rec->m_raceID));
    memcpy(rec->m_normalSoundID, &row[0x000C], sizeof(rec->m_normalSoundID));
    memcpy(rec->m_pissedSoundID, &row[0x0014], sizeof(rec->m_pissedSoundID));
}
#endif
