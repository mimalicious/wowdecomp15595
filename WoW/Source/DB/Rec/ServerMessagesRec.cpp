#include "WoW/Source/DB/Rec/ServerMessagesRec.h"

// STATIC: 0x00C22650/0x140A17540
const DBCMeta ServerMessagesRec::s_meta = { "DBFilesClient\\ServerMessages.dbc", 2, 8, ServerMessagesRec::FixupStrings, 0 };

// PROVEN: 30436f2e IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ServerMessagesRec::FixupStrings(void* record, const char* strings) {
    ServerMessagesRec* rec = static_cast<ServerMessagesRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
