#include "WoW/Source/DB/Rec/JournalEncounterSectionRec.h"

// STATIC: 0x00C23BAC/0x140A18FC8
const DBCMeta JournalEncounterSectionRec::s_meta = { "DBFilesClient\\JournalEncounterSection.dbc", 14, 56, JournalEncounterSectionRec::FixupStrings, 0 };

// OFFSET: 0x00A2EB10/0x1407D8290
void JournalEncounterSectionRec::FixupStrings(void* record, const char* strings) {
    JournalEncounterSectionRec* rec = static_cast<JournalEncounterSectionRec*>(record);
    DBCFixupString(rec->unk_0024, strings);
    DBCFixupString(rec->unk_0028, strings);
}
