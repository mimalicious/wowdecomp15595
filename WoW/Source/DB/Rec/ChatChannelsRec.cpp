#include "WoW/Source/DB/Rec/ChatChannelsRec.h"

// STATIC: %0x00B46FB8
const DBCMeta ChatChannelsRec::s_meta = { "DBFilesClient\\ChatChannels.dbc", 5, 20, ChatChannelsRec::FixupStrings, 0 };

// OFFSET: %0x0097C440
void ChatChannelsRec::FixupStrings(void* record, const char* strings) {
    ChatChannelsRec* rec = static_cast<ChatChannelsRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_shortcut = &strings[reinterpret_cast<uintptr_t>(rec->m_shortcut)];
}

#if defined(WOWDECOMP_ARCH_64)
void ChatChannelsRec::ConvertRow(ChatChannelsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    memcpy(&rec->m_factionGroup, &row[0x0008], sizeof(rec->m_factionGroup));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->m_shortcut = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
}
#endif
