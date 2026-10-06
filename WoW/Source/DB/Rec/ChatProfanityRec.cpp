#include "WoW/Source/DB/Rec/ChatProfanityRec.h"

// STATIC: 0x00C1E6E0/0x140A12968
const DBCMeta ChatProfanityRec::s_meta = { "DBFilesClient\\ChatProfanity.dbc", 3, 12, ChatProfanityRec::FixupStrings, 0 };

// PROVEN: f1019073 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ChatProfanityRec::FixupStrings(void* record, const char* strings) {
    ChatProfanityRec* rec = static_cast<ChatProfanityRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
