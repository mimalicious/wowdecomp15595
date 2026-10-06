#include "WoW/Source/DB/Rec/JournalEncounterRec.h"

// STATIC: 0x00C23C68/0x140A190A0
const DBCMeta JournalEncounterRec::s_meta = { "DBFilesClient\\JournalEncounter.dbc", 10, 44, JournalEncounterRec::FixupStrings, 0 };

// OFFSET: 0x00A2EB10/0x1407D8290
void JournalEncounterRec::FixupStrings(void* record, const char* strings) {
    JournalEncounterRec* rec = static_cast<JournalEncounterRec*>(record);
    DBCFixupString(rec->unk_0024, strings);
    DBCFixupString(rec->unk_0028, strings);
}
