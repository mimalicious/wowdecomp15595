#include "WoW/Source/DB/Rec/JournalInstanceRec.h"

// STATIC: 0x00C23AB8/0x140A18EB8
const DBCMeta JournalInstanceRec::s_meta = { "DBFilesClient\\JournalInstance.dbc", 9, 36, JournalInstanceRec::FixupStrings, 0 };

// OFFSET: 0x00A2EAF0/0x1407D8250
void JournalInstanceRec::FixupStrings(void* record, const char* strings) {
    JournalInstanceRec* rec = static_cast<JournalInstanceRec*>(record);
    DBCFixupString(rec->unk_001C, strings);
    DBCFixupString(rec->unk_0020, strings);
}
