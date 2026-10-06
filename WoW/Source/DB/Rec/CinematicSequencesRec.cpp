#include "WoW/Source/DB/Rec/CinematicSequencesRec.h"

// STATIC: %0x00B46F4C
const DBCMeta CinematicSequencesRec::s_meta = { "DBFilesClient\\CinematicSequences.dbc", 3, 40, CinematicSequencesRec::FixupStrings, 0 };

// PROVEN: a392b303 IDENTICAL
// OFFSET: 0x00401000
void CinematicSequencesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CinematicSequencesRec::ConvertRow(CinematicSequencesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_soundID, &row[0x0004], sizeof(rec->m_soundID));
    memcpy(rec->m_camera, &row[0x0008], sizeof(rec->m_camera));
}
#endif
