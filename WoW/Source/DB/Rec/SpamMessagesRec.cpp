#include "WoW/Source/DB/Rec/SpamMessagesRec.h"

// STATIC: %0x00B435C4
const DBCMeta SpamMessagesRec::s_meta = { "DBFilesClient\\SpamMessages.dbc", 2, 8, SpamMessagesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SpamMessagesRec::FixupStrings(void* record, const char* strings) {
    SpamMessagesRec* rec = static_cast<SpamMessagesRec*>(record);
    rec->m_text = &strings[reinterpret_cast<uintptr_t>(rec->m_text)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpamMessagesRec::ConvertRow(SpamMessagesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_text = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
