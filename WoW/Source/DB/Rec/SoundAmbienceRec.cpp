#include "WoW/Source/DB/Rec/SoundAmbienceRec.h"

// STATIC: %0x00B45490
const DBCMeta SoundAmbienceRec::s_meta = { "DBFilesClient\\SoundAmbience.dbc", 2, 12, SoundAmbienceRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void SoundAmbienceRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SoundAmbienceRec::ConvertRow(SoundAmbienceRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_ambienceID, &row[0x0004], sizeof(rec->m_ambienceID));
}
#endif
