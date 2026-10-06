#include "WoW/Source/DB/Rec/ChatProfanityRec.h"

// STATIC: %0x00B43E9C
const DBCMeta ChatProfanityRec::s_meta = { "DBFilesClient\\ChatProfanity.dbc", 3, 12, ChatProfanityRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ChatProfanityRec::FixupStrings(void* record, const char* strings) {
    ChatProfanityRec* rec = static_cast<ChatProfanityRec*>(record);
    rec->m_text = &strings[reinterpret_cast<uintptr_t>(rec->m_text)];
}

#if defined(WOWDECOMP_ARCH_64)
void ChatProfanityRec::ConvertRow(ChatProfanityRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_text = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_language, &row[0x0008], sizeof(rec->m_language));
}
#endif
