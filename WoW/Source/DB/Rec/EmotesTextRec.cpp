#include "WoW/Source/DB/Rec/EmotesTextRec.h"

// STATIC: %0x00B46B68
const DBCMeta EmotesTextRec::s_meta = { "DBFilesClient\\EmotesText.dbc", 4, 76, EmotesTextRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void EmotesTextRec::FixupStrings(void* record, const char* strings) {
    EmotesTextRec* rec = static_cast<EmotesTextRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void EmotesTextRec::ConvertRow(EmotesTextRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_emoteID, &row[0x0008], sizeof(rec->m_emoteID));
    memcpy(rec->m_emoteText, &row[0x000C], sizeof(rec->m_emoteText));
}
#endif
