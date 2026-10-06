#include "WoW/Source/DB/Rec/ServerMessagesRec.h"

// STATIC: %0x00B4561C
const DBCMeta ServerMessagesRec::s_meta = { "DBFilesClient\\ServerMessages.dbc", 2, 8, ServerMessagesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ServerMessagesRec::FixupStrings(void* record, const char* strings) {
    ServerMessagesRec* rec = static_cast<ServerMessagesRec*>(record);
    rec->m_text = &strings[reinterpret_cast<uintptr_t>(rec->m_text)];
}

#if defined(WOWDECOMP_ARCH_64)
void ServerMessagesRec::ConvertRow(ServerMessagesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_text = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
