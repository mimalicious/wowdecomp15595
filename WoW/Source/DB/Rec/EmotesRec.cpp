#include "WoW/Source/DB/Rec/EmotesRec.h"

// STATIC: %0x00B46C08
const DBCMeta EmotesRec::s_meta = { "DBFilesClient\\Emotes.dbc", 7, 28, EmotesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void EmotesRec::FixupStrings(void* record, const char* strings) {
    EmotesRec* rec = static_cast<EmotesRec*>(record);
    rec->m_emoteSlashCommand = &strings[reinterpret_cast<uintptr_t>(rec->m_emoteSlashCommand)];
}

#if defined(WOWDECOMP_ARCH_64)
void EmotesRec::ConvertRow(EmotesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_emoteSlashCommand = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_animID, &row[0x0008], sizeof(rec->m_animID));
    memcpy(&rec->m_emoteFlags, &row[0x000C], sizeof(rec->m_emoteFlags));
    memcpy(&rec->m_emoteSpecProc, &row[0x0010], sizeof(rec->m_emoteSpecProc));
    memcpy(&rec->m_emoteSpecProcParam, &row[0x0014], sizeof(rec->m_emoteSpecProcParam));
    memcpy(&rec->m_eventSoundID, &row[0x0018], sizeof(rec->m_eventSoundID));
}
#endif
