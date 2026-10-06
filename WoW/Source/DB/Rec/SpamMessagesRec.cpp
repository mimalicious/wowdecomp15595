#include "WoW/Source/DB/Rec/SpamMessagesRec.h"

// STATIC: 0x00C1DE40/0x140A11ED0
const DBCMeta SpamMessagesRec::s_meta = { "DBFilesClient\\SpamMessages.dbc", 2, 8, SpamMessagesRec::FixupStrings, 0 };

// PROVEN: dba6a13e IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpamMessagesRec::FixupStrings(void* record, const char* strings) {
    SpamMessagesRec* rec = static_cast<SpamMessagesRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
