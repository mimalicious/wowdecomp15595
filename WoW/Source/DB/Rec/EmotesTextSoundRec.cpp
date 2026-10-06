#include "WoW/Source/DB/Rec/EmotesTextSoundRec.h"

// STATIC: %0x00B46BA0
const DBCMeta EmotesTextSoundRec::s_meta = { "DBFilesClient\\EmotesTextSound.dbc", 5, 20, EmotesTextSoundRec::FixupStrings, 0 };

// PROVEN: c4610e4c IDENTICAL
// OFFSET: 0x00401000
void EmotesTextSoundRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void EmotesTextSoundRec::ConvertRow(EmotesTextSoundRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_emotesTextID, &row[0x0004], sizeof(rec->m_emotesTextID));
    memcpy(&rec->m_raceID, &row[0x0008], sizeof(rec->m_raceID));
    memcpy(&rec->m_sexID, &row[0x000C], sizeof(rec->m_sexID));
    memcpy(&rec->m_soundID, &row[0x0010], sizeof(rec->m_soundID));
}
#endif
