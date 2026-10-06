#include "WoW/Source/DB/Rec/JournalEncounterCreatureRec.h"

// STATIC: 0x00C23D8C/0x140A19210
const DBCMeta JournalEncounterCreatureRec::s_meta = { "DBFilesClient\\JournalEncounterCreature.dbc", 7, 28, JournalEncounterCreatureRec::FixupStrings, 0 };

// PROVEN: 5f39c62b IDENTICAL/NONE
// OFFSET: 0x00A2EB30/0x1407D7E90
void JournalEncounterCreatureRec::FixupStrings(void* record, const char* strings) {
    JournalEncounterCreatureRec* rec = static_cast<JournalEncounterCreatureRec*>(record);
    DBCFixupString(rec->unk_0014, strings);
    DBCFixupString(rec->unk_0018, strings);
}
