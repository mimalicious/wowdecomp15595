#include "WoW/Source/DB/Rec/SoundEntriesFallbacksRec.h"

// STATIC: %0x00B40A24
const DBCMeta SoundEntriesFallbacksRec::s_meta = { "DBFilesClient\\SoundEntriesFallbacks.dbc", 3, 12, SoundEntriesFallbacksRec::FixupStrings, 0 };

// PROVEN: 1d63464e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SoundEntriesFallbacksRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SoundEntriesFallbacksRec::ConvertRow(SoundEntriesFallbacksRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_soundEntriesID, &row[0x0004], sizeof(rec->m_soundEntriesID));
    memcpy(&rec->m_fallbackSoundEntriesID, &row[0x0008], sizeof(rec->m_fallbackSoundEntriesID));
}
#endif
