#include "WoW/Source/DB/Rec/JournalEncounterItemRec.h"

// STATIC: 0x00C23CEC/0x140A19148
const DBCMeta JournalEncounterItemRec::s_meta = { "DBFilesClient\\JournalEncounterItem.dbc", 5, 20, JournalEncounterItemRec::FixupStrings, 0 };

// PROVEN: ac5914aa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void JournalEncounterItemRec::FixupStrings(void* record, const char* strings) {
}
